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

#include "Mesh.h"
#include "RenderNode.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"

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

	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	for (auto p : m_ShaderContainer) { RELEASE(p); }
	for (auto p : m_MaterialContainer) { RELEASE(p); }
	for (auto p : m_RenderNodeContainer) { RELEASE(p); }

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
		RELEASE(m_MaterialUniformBuffers[i]);
	}

	RELEASE(m_VKCommandPool);

	// 清理各种mgr

	m_GPUProfilerMgr->WriteToFile();
	RELEASE(m_GPUProfilerMgr);

	GetProfilerMgr().WriteToFile();

	ReleaseProfilerMgr();

	ReleaseTimeMgr();

	// 最后清理Driver

	ReleaseVulkanDriver();
}

void Engine::InitEngine()
{
	// 先初始化Driver

	CreateVulkanDriver();
	auto& driver = GetVulkanDriver();

	// 初始化各种mgr

	m_GPUProfilerMgr = driver.CreateGPUProfilerMgr();

	CreateTimeMgr();

	CreateProfilerMgr();

	// 再初始化父类

	m_VKCommandPool = driver.CreateVKCommandPool();

	m_FrameResources.resize(FrameResourcesCount);
	for (size_t i = 0; i < FrameResourcesCount; ++i) {
		m_FrameResources[i].framebuffer = nullptr; // 动态创建framebuffer
		m_FrameResources[i].commandBuffer = driver.CreateVKCommandBuffer(m_VKCommandPool);
		m_FrameResources[i].imageAvailableSemaphore = driver.CreateVKSemaphore();
		m_FrameResources[i].finishedRenderingSemaphore = driver.CreateVKSemaphore();
		m_FrameResources[i].fence = driver.CreateVKFence(true);
	}

	auto& dp = GetDeviceProperties();
	uint32_t minUboAlignment = static_cast<uint32_t>(dp.deviceProperties.limits.minUniformBufferOffsetAlignment);
	m_ObjectUBODynamicAlignment = ((sizeof(ObjectUniform) + minUboAlignment - 1) / minUboAlignment) * minUboAlignment;
	m_MaterialUBODynamicAlignment = ((sizeof(MaterialUniform) + minUboAlignment - 1) / minUboAlignment) * minUboAlignment;

	m_PassUniformBuffers.resize(FrameResourcesCount);
	m_ObjectUniformBuffers.resize(FrameResourcesCount);
	m_MaterialUniformBuffers.resize(FrameResourcesCount);
	for (size_t i = 0; i < FrameResourcesCount; ++i) {
		m_PassUniformBuffers[i] = driver.CreateVKBuffer(sizeof(PassUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_PassUniformBuffers[i]->Map();

		m_ObjectUniformBuffers[i] = driver.CreateVKBuffer(m_ObjectUBODynamicAlignment * m_ObjectUniformNum, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_ObjectUniformBuffers[i]->Map();

		m_MaterialUniformBuffers[i] = driver.CreateVKBuffer(m_MaterialUBODynamicAlignment * m_MaterialUniformNum, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_MaterialUniformBuffers[i]->Map();
	}

	m_Imgui = new Imgui();

	// 最后初始化子类

	Init();
}

void Engine::TickEngine()
{
	// 更新时间
	auto& timeMgr = GetTimeMgr();
	timeMgr.Tick();

	PROFILER(Engine_TickEngine);

	// 更新游戏逻辑
	Tick();
	for (auto node : m_RenderNodeContainer) {
		UpdateObjectUniformBuffer(node);
	}
	for (auto mat : m_MaterialContainer) {
		UpdateMaterialUniformBuffer(mat);
	}

	// 更新UI逻辑
	m_Imgui->Prepare();
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
		m_ObjectUniformBuffers[m_CurrFrameIndex]->Copy(&node->GetWorldMatrix(), m_ObjectUBODynamicAlignment * node->GetDUBIndex(), m_ObjectUBODynamicAlignment);
		node->Clean();
	}
}

void Engine::UpdateMaterialUniformBuffer(Material * material)
{
	MaterialUniform matUniform;
	matUniform.diffuseAlbedo = material->GetDiffuseAlbedo();
	matUniform.fresnelR0 = material->GetFresnelR0();
	matUniform.roughness = material->GetRoughness();
	matUniform.matTransform = material->GetMatTransform();

	if (material->IsDirty()) {
		m_MaterialUniformBuffers[m_CurrFrameIndex]->Copy(&matUniform, m_MaterialUBODynamicAlignment * material->GetDUBIndex(), m_MaterialUBODynamicAlignment);
		material->Clean();
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

VKBuffer * Engine::GetCurrMaterialUniformBuffer()
{
	return m_MaterialUniformBuffers[m_CurrFrameIndex];
}

uint32_t Engine::GetObjectUBODynamicAlignment()
{
	return m_ObjectUBODynamicAlignment;
}

uint32_t Engine::GetMaterialUBODynamicAlignment()
{
	return m_MaterialUBODynamicAlignment;
}

Mesh * Engine::CreateMesh()
{
	Mesh* mesh = new Mesh();
	m_MeshContainer.push_back(mesh);

	return mesh;
}

Texture * Engine::CreateTexture()
{
	Texture* texture = new Texture();
	m_TextureContainer.push_back(texture);

	return texture;
}

Shader * Engine::CreateShader()
{
	Shader* shader = new Shader();
	m_ShaderContainer.push_back(shader);

	return shader;
}

Material * Engine::CreateMaterial()
{
	Material* material = new Material(static_cast<uint32_t>(m_MaterialContainer.size()));
	m_MaterialContainer.push_back(material);

	return material;
}

RenderNode * Engine::CreateRenderNode()
{
	RenderNode* renderNode = new RenderNode(static_cast<uint32_t>(m_RenderNodeContainer.size()));
	m_RenderNodeContainer.push_back(renderNode);

	return renderNode;
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