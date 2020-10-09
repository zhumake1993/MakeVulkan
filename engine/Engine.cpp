#include "Engine.h"

#include "VulkanDriver.h"

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

	// 先清理子类

	CleanUp();

	// 再清理父类

	RELEASE(m_Imgui);

	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		RELEASE(m_FrameResources[i].framebuffer);
		RELEASE(m_FrameResources[i].commandBuffer);
		RELEASE(m_FrameResources[i].imageAvailableSemaphore);
		RELEASE(m_FrameResources[i].finishedRenderingSemaphore);
		RELEASE(m_FrameResources[i].fence);
	}

	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		RELEASE(m_PassUniformBuffers[i]);
		RELEASE(m_ObjectUniformBuffers[i]);
	}

	RELEASE(m_VKCommandPool);

	// 最后清理Driver

	ReleaseVulkanDriver();
}

void Engine::InitEngine()
{
	// 先初始化Driver

	CreateVulkanDriver();

	auto& driver = GetVulkanDriver();

	// 再初始化父类

	m_VKCommandPool = driver.CreateVKCommandPool();

	m_FrameResources.resize(global::frameResourcesCount);
	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_FrameResources[i].framebuffer = nullptr; // 动态创建framebuffer
		m_FrameResources[i].commandBuffer = driver.CreateVKCommandBuffer(m_VKCommandPool);
		m_FrameResources[i].imageAvailableSemaphore = driver.CreateVKSemaphore();
		m_FrameResources[i].finishedRenderingSemaphore = driver.CreateVKSemaphore();
		m_FrameResources[i].fence = driver.CreateVKFence(true);
	}

	m_PassUniformBuffers.resize(global::frameResourcesCount);
	m_ObjectUniformBuffers.resize(global::frameResourcesCount);
	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_PassUniformBuffers[i] = driver.CreateVKBuffer(sizeof(PassUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_PassUniformBuffers[i]->Map();

		m_ObjectUniformBuffers[i] = driver.CreateVKBuffer(sizeof(ObjectUniform) * m_ObjectUniformNum, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_ObjectUniformBuffers[i]->Map();
	}

	m_Imgui = new Imgui();

	// 最后初始化子类

	Init();
}

void Engine::TickEngine()
{
	// 更新时间
	m_TimeMgr.Tick();
	float deltaTime = m_TimeMgr.GetDeltaTime();

	// 更新游戏逻辑
	Tick(deltaTime);

	// 更新UI逻辑
	m_Imgui->Prepare(deltaTime);
	TickUI();
	ImGui::Render();

	// 更新输入
	input.Tick();

	// 更新UI顶点计算
	m_Imgui->Tick();

	// 更新DescriptorSetMgr
	auto& driver = GetVulkanDriver();
	driver.GetDescriptorSetMgr().Tick();

	WaitForPresent();

	// 提交draw call
	RecordCommandBuffer(m_FrameResources[m_CurrFrameIndex].commandBuffer);

	// 提交UI draw call
	//m_Imgui->RecordCommandBuffer(m_FrameResources[m_CurrFrameIndex].commandBuffer);

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

void Engine::UpdateObjectUniformBuffer(void * data, uint32_t index)
{
	m_ObjectUniformBuffers[m_CurrFrameIndex]->Copy(data, sizeof(ObjectUniform) * index, sizeof(ObjectUniform));
}

VKBuffer * Engine::GetCurrPassUniformBuffer()
{
	return m_PassUniformBuffers[m_CurrFrameIndex];
}

VKBuffer * Engine::GetCurrObjectUniformBuffer()
{
	return m_ObjectUniformBuffers[m_CurrFrameIndex];
}

void Engine::WaitForPresent()
{
	auto& driver = GetVulkanDriver();

	auto& currFrameResource = m_FrameResources[m_CurrFrameIndex];

	currFrameResource.fence->Wait();
	currFrameResource.fence->Reset();

	driver.AcquireNextImage(currFrameResource.imageAvailableSemaphore);
}

void Engine::Present()
{
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

	m_CurrFrameIndex = (m_CurrFrameIndex + 1) % global::frameResourcesCount;
}