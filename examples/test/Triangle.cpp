#include "Triangle.h"

#include "InputManager.h"

#include "VulkanDriver.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"

#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

#include "VulkanCommandBuffer.h"

void ConfigGlobalSettings() {
	// 添加单独的实例级层

	// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
	// Note that on Android this layer requires at least NDK r20
#if defined(_WIN32)
	// 目前我用的NDK r19
	global::enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// 添加单独的实例级拓展

	// 这个暂时搁置
	//global::enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	global::enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
	global::enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	global::enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

	global::enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void Triangle::CleanUp()
{
	m_Image->CleanUp();
	m_VertexBuffer->CleanUp();
	m_IndexBuffer->CleanUp();

	m_VulkanDescriptorPool->CleanUp();
	m_VulkanDescriptorSetLayout->CleanUp();
	m_VulkanPipeline->CleanUp();
	m_VulkanPipelineLayout->CleanUp();
	m_VulkanRenderPass->CleanUp();
}

void Triangle::Init()
{
	// 摄像机
	m_Camera.LookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera.SetLens(glm::radians(60.0f), 1.0f * global::windowWidth / global::windowHeight, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera.SetSpeed(1.0f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera.SetSpeed(0.001f, 0.005f);
#endif

	PrepareVertices();
	PrepareTextures();
	PrepareDescriptorSet();
	CreatePipeline();
}

void Triangle::Tick()
{
	m_FrameIndex++;
	m_AccumulateCounter++;

	auto timestamp = std::chrono::high_resolution_clock::now();
	float deltaTime = (timestamp - lastTimestamp).count() / 1000000000.0f;
	lastTimestamp = timestamp;
	m_AccumulateTime += deltaTime;
	if (m_AccumulateTime >= 0.5f) {
		m_FPS = m_AccumulateCounter / m_AccumulateTime;
		m_AccumulateCounter = 0;
		m_AccumulateTime = 0;

		LOG("m_FPS: %f\n", m_FPS);
	}

	// 每帧的逻辑

	m_Camera.Update(deltaTime);

#if defined(_WIN32)
	input.oldPos = input.pos;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	input.oldPos0 = input.pos0;
	input.oldPos1 = input.pos1;
#endif

	m_UniformBuffer.view = m_Camera.GetView();
	m_UniformBuffer.proj = m_Camera.GetProj();

	auto driver = GetVulkanDriver();

	driver.UpdateUniformBuffer(&m_UniformBuffer, sizeof(UniformBuffer));
}

void Triangle::RecordCommandBuffer(VulkanCommandBuffer * vulkanCommandBuffer)
{
	auto driver = GetVulkanDriver();

	VkClearValue clearValue = {};
	clearValue.color = { 0.0f, 0.0f, 0.2f, 1.0f };
	clearValue.depthStencil = { 0.0f, 0 };

	VkRect2D area = {};
	area.offset.x = 0;
	area.offset.y = 0;
	area.extent.width = global::windowWidth;
	area.extent.height = global::windowHeight;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(global::windowWidth);
	viewport.height = static_cast<float>(global::windowHeight);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vulkanCommandBuffer->Begin();

	vulkanCommandBuffer->BeginRenderPass(m_VulkanRenderPass, driver.CreateFramebuffer(m_VulkanRenderPass), area, clearValue);

	vulkanCommandBuffer->SetViewport(viewport);

	vulkanCommandBuffer->SetScissor(area);

	vulkanCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline);

	vulkanCommandBuffer->BindVertexBuffer(0, m_VertexBuffer);

	vulkanCommandBuffer->BindIndexBuffer(m_IndexBuffer, VK_INDEX_TYPE_UINT32);

	vulkanCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipelineLayout, m_VulkanDescriptorSet);

	vulkanCommandBuffer->DrawIndexed(3, 1, 0, 0, 1);

	vulkanCommandBuffer->EndRenderPass();

	vulkanCommandBuffer->End();
}

void Triangle::PrepareVertices()
{
	auto driver = GetVulkanDriver();

	// Vertex buffer
	std::vector<float> vertexBuffer =
	{
		  -0.5f, 0.5f, 0.0f,  1.0f ,
		  1.0f, 0.0f, 0.0f,  0.0f  ,
		  0.0f,  0.0f  ,

		   0.5f, 0.5f, 0.0f,  1.0f ,
		   0.0f, 1.0f, 0.0f,  0.0f  ,
		   1.0f,  0.0f  ,

		   0.5f,  -0.5f, 0.0f,  1.0f ,
		   0.0f, 0.0f, 1.0f,  0.0f,
		   1.0f,  1.0f  ,
	};
	//Model model;
	//model.loadFromFile(GetAssetPath() + "models/viking_room.obj");
	//auto vertexBuffer = model.m_Vertices;

	uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(vertexBuffer[0]);

	m_VertexBuffer = driver.CreateVulkanBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	driver.UploadVulkanBuffer(m_VertexBuffer, vertexBuffer.data(), vertexBufferSize);

	// Index buffer
	std::vector<uint32_t> indexBuffer = { 0, 1, 2 };
	//auto indexBuffer = model.m_Indices;
	uint32_t indexBufferSize = static_cast<uint32_t>(indexBuffer.size()) * sizeof(uint32_t);

	m_IndexBuffer = driver.CreateVulkanBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	driver.UploadVulkanBuffer(m_IndexBuffer, indexBuffer.data(), indexBufferSize);
}

void Triangle::PrepareTextures()
{
	auto driver = GetVulkanDriver();

	uint32_t width = 0, height = 0, dataSize = 0;
	std::vector<char> imageData = GetImageData(GetAssetPath() + "textures/texture.png", 4, &width, &height, nullptr, &dataSize);
	if (imageData.size() == 0) {
		assert(false);
	}

	m_Image = driver.CreateVulkanImage(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

	driver.UploadVulkanImage(m_Image, imageData.data(), dataSize);
}

void Triangle::PrepareDescriptorSet()
{
	auto driver = GetVulkanDriver();

	m_VulkanDescriptorSetLayout = driver.CreateVulkanDescriptorSetLayout();
	m_VulkanDescriptorSetLayout->AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_VulkanDescriptorSetLayout->AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
	m_VulkanDescriptorSetLayout->Create();

	m_VulkanDescriptorPool = driver.CreateVulkanDescriptorPool();
	m_VulkanDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	m_VulkanDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	m_VulkanDescriptorPool->Create(1);

	m_VulkanDescriptorSet = m_VulkanDescriptorPool->AllocateDescriptorSet(m_VulkanDescriptorSetLayout);

	std::vector<DescriptorSetUpdater*> descriptorSetUpdaters(2);

	descriptorSetUpdaters[0] = new DescriptorSetUpdater(m_VulkanDescriptorSet, 0, 0);
	descriptorSetUpdaters[0]->AddImage(m_Image);

	descriptorSetUpdaters[1] = new DescriptorSetUpdater(m_VulkanDescriptorSet, 1, 0);
	descriptorSetUpdaters[1]->AddBuffer(driver.GetCurrUniformBuffer());

	driver.UpdateDescriptorSets(descriptorSetUpdaters);
}

void Triangle::CreatePipeline()
{
	auto driver = GetVulkanDriver();

	m_VulkanRenderPass = driver.CreateVulkanRenderPass();

	// std::make_unique 需要C++14的支持，这里使用构造函数更保险
	std::shared_ptr<VulkanShaderModule> vulkanShaderModuleVert = std::shared_ptr<VulkanShaderModule>(driver.CreateVulkanShaderModule(GetAssetPath() + "shaders/shader.vert.spv"));
	std::shared_ptr<VulkanShaderModule> vulkanShaderModuleFrag = std::shared_ptr<VulkanShaderModule>(driver.CreateVulkanShaderModule(GetAssetPath() + "shaders/shader.frag.spv"));

	PipelineCI pipelineCI;

	pipelineCI.shaderStage.vertShaderModule = vulkanShaderModuleVert;
	pipelineCI.shaderStage.fragShaderModule = vulkanShaderModuleFrag;

	pipelineCI.vertexInputState.vertexLayout.push_back(kVertexFormatFloat32x4);
	pipelineCI.vertexInputState.vertexLayout.push_back(kVertexFormatFloat32x4);
	pipelineCI.vertexInputState.vertexLayout.push_back(kVertexFormatFloat32x2);

	pipelineCI.rasterizationState.cullMode = VK_CULL_MODE_NONE;

	pipelineCI.dynamicState.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	pipelineCI.dynamicState.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

	m_VulkanPipelineLayout = driver.CreateVulkanPipelineLayout(m_VulkanDescriptorSetLayout);

	pipelineCI.Configure(m_VulkanPipelineLayout, m_VulkanRenderPass);

	m_VulkanPipeline = driver.CreateVulkanPipeline(pipelineCI);
}