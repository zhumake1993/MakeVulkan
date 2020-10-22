#include "Engine.h"

#include "VulkanDriver.h"
#include "DeviceProperties.h"

#include "VKCommandPool.h"

#include "VKCommandBuffer.h"
#include "VKSemaphore.h"
#include "VKFence.h"
#include "VKFramebuffer.h"

#include "VKBuffer.h"
#include "VKImage.h"

#include "DescriptorSetMgr.h"

#include "VKShaderModule.h"
#include "VKPipelineLayout.h"
#include "VKPipeline.h"
#include "VKRenderPass.h"

#include "Tools.h"
#include "InputManager.h"
#include "Gui.h"
#include "TimeMgr.h"
#include "ProfilerMgr.h"
#include "GPUProfilerMgr.h"

#include "RenderNode.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::CleanUpEngine()
{
	auto& driver = GetVulkanDriver();
	driver.DeviceWaitIdle();

	// ����������

	CleanUp();

	// ��������

	RELEASE(m_Imgui);

	for (size_t i = 0; i < FrameResourcesCount; ++i) {
		RELEASE(m_FrameResources[i].framebuffer);
		RELEASE(m_FrameResources[i].commandBuffer);
		RELEASE(m_FrameResources[i].imageAvailableSemaphore);
		RELEASE(m_FrameResources[i].finishedRenderingSemaphore);
		RELEASE(m_FrameResources[i].fence);
	}

	for (size_t i = 0; i < FrameResourcesCount; ++i) {
		RELEASE(m_PassUniformBuffers[i]);
		RELEASE(m_ObjectUniformBuffers[i]);
	}

	RELEASE(m_VKCommandPool);

	// �������mgr

	m_GPUProfilerMgr->WriteToFile();
	RELEASE(m_GPUProfilerMgr);

	GetProfilerMgr().WriteToFile();

	ReleaseProfilerMgr();

	ReleaseTimeMgr();

	// �������Driver

	ReleaseVulkanDriver();
}

void Engine::InitEngine()
{
	// �ȳ�ʼ��Driver

	CreateVulkanDriver();
	auto& driver = GetVulkanDriver();

	// ��ʼ������mgr

	m_GPUProfilerMgr = driver.CreateGPUProfilerMgr();

	CreateTimeMgr();

	CreateProfilerMgr();

	// �ٳ�ʼ������

	m_VKCommandPool = driver.CreateVKCommandPool();

	m_FrameResources.resize(FrameResourcesCount);
	for (size_t i = 0; i < FrameResourcesCount; ++i) {
		m_FrameResources[i].framebuffer = nullptr; // ��̬����framebuffer
		m_FrameResources[i].commandBuffer = driver.CreateVKCommandBuffer(m_VKCommandPool);
		m_FrameResources[i].imageAvailableSemaphore = driver.CreateVKSemaphore();
		m_FrameResources[i].finishedRenderingSemaphore = driver.CreateVKSemaphore();
		m_FrameResources[i].fence = driver.CreateVKFence(true);
	}

	auto& dp = GetDeviceProperties();
	uint32_t minUboAlignment = static_cast<uint32_t>(dp.deviceProperties.limits.minUniformBufferOffsetAlignment);
	m_UBODynamicAlignment = ((sizeof(ObjectUniform) + minUboAlignment - 1) / minUboAlignment) * minUboAlignment;

	m_PassUniformBuffers.resize(FrameResourcesCount);
	m_ObjectUniformBuffers.resize(FrameResourcesCount);
	for (size_t i = 0; i < FrameResourcesCount; ++i) {
		m_PassUniformBuffers[i] = driver.CreateVKBuffer(sizeof(PassUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_PassUniformBuffers[i]->Map();

		m_ObjectUniformBuffers[i] = driver.CreateVKBuffer(m_UBODynamicAlignment * m_ObjectUniformNum, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_ObjectUniformBuffers[i]->Map();
	}

	m_Imgui = new Imgui();

	// ����ʼ������

	Init();
}

void Engine::TickEngine()
{
	// ����ʱ��
	auto& timeMgr = GetTimeMgr();
	timeMgr.Tick();

	PROFILER(Engine_TickEngine);

	// ������Ϸ�߼�
	Tick();

	// ����UI�߼�
	m_Imgui->Prepare();
	TickUI();
	ImGui::Render();

	// ��������
	input.Tick();

	// ����UI�������
	m_Imgui->Tick();

	// ����DescriptorSetMgr
	auto& driver = GetVulkanDriver();
	driver.GetDescriptorSetMgr().Tick();

	WaitForPresent();

	// �ύdraw call
	RecordCommandBuffer(m_FrameResources[m_CurrFrameIndex].commandBuffer);

	// �ύUI draw call
	//m_Imgui->RecordCommandBuffer(m_FrameResources[m_CurrFrameIndex].commandBuffer);

	m_GPUProfilerMgr->Tick();

	Present();
}

VKFramebuffer * Engine::RebuildFramebuffer(VKRenderPass * vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height)
{
	auto& driver = GetVulkanDriver();

	RELEASE(m_FrameResources[m_CurrFrameIndex].framebuffer);

	m_FrameResources[m_CurrFrameIndex].framebuffer = driver.CreateVKFramebuffer(vkRenderPass, color, depth, width, height);

	return m_FrameResources[m_CurrFrameIndex].framebuffer;
}

void Engine::UpdatePassUniformBuffer(void * data)
{
	m_PassUniformBuffers[m_CurrFrameIndex]->Copy(data, 0, sizeof(PassUniform));
}

void Engine::UpdateObjectUniformBuffer(RenderNode* node)
{
	if (node->IsDirty()) {
		m_ObjectUniformBuffers[m_CurrFrameIndex]->Copy(&node->GetWorldMatrix(), m_UBODynamicAlignment * node->GetObjectUBIndex(), m_UBODynamicAlignment);
		node->Clean();
	}
}

VKBuffer * Engine::GetCurrPassUniformBuffer()
{
	return m_PassUniformBuffers[m_CurrFrameIndex];
}

VKBuffer * Engine::GetCurrObjectUniformBuffer()
{
	return m_ObjectUniformBuffers[m_CurrFrameIndex];
}

uint32_t Engine::GetUBODynamicAlignment()
{
	return m_UBODynamicAlignment;
}

void Engine::WaitForPresent()
{
	PROFILER(Engine_WaitForPresent);

	auto& driver = GetVulkanDriver();

	auto& currFrameResource = m_FrameResources[m_CurrFrameIndex];

	currFrameResource.fence->Wait();
	currFrameResource.fence->Reset();

	driver.AcquireNextImage(currFrameResource.imageAvailableSemaphore);
}

void Engine::Present()
{
	PROFILER(Engine_Present);

	auto& driver = GetVulkanDriver();

	auto& currFrameResource = m_FrameResources[m_CurrFrameIndex];

	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &currFrameResource.imageAvailableSemaphore->semaphore;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &currFrameResource.commandBuffer->commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &currFrameResource.finishedRenderingSemaphore->semaphore;

	driver.QueueSubmit(submitInfo, currFrameResource.fence);
	driver.QueuePresent(currFrameResource.finishedRenderingSemaphore);

	m_CurrFrameIndex = (m_CurrFrameIndex + 1) % FrameResourcesCount;
}