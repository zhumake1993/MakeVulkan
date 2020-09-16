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

#include "Application.h"
#include "Camera.h"
#include "Mesh.h"
#include "RenderNode.h"

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
	RELEASE(m_Mesh);

	RELEASE(m_Image);

	RELEASE(m_RenderNode);

	RELEASE(m_VulkanDescriptorSetLayout);
	RELEASE(m_VulkanDescriptorSet);
	RELEASE(m_VulkanDescriptorPool);

	RELEASE(m_VulkanPipeline);
	RELEASE(m_VulkanPipelineLayout);
	RELEASE(m_VulkanRenderPass);

	RELEASE(m_Camera);

	RELEASE(m_DepthImage);
}

void Triangle::Init()
{
	auto& driver = GetVulkanDriver();

	m_DepthFormat = driver.GetDepthFormat();
	m_DepthImage = driver.CreateVulkanImage(VK_IMAGE_TYPE_2D, m_DepthFormat, global::windowWidth, global::windowHeight, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	// 摄像机
	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetLens(glm::radians(60.0f), 1.0f * global::windowWidth / global::windowHeight, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera->SetSpeed(1.0f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera->SetSpeed(0.001f, 0.005f);
#endif

	PrepareResources();
	PrepareDescriptorSet();
	CreatePipeline();
}

void Triangle::Tick()
{
	auto& driver = GetVulkanDriver();

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

	m_Camera->Update(deltaTime);

#if defined(_WIN32)
	input.oldPos = input.pos;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	input.oldPos0 = input.pos0;
	input.oldPos1 = input.pos1;
#endif

	m_PassUniform.view = m_Camera->GetView();
	m_PassUniform.proj = m_Camera->GetProj();

	driver.UpdatePassUniformBuffer(&m_PassUniform);

	{
		if (m_RenderNode->m_NumFramesDirty > 0) {
			driver.UpdateObjectUniformBuffer(&m_RenderNode->m_World, m_RenderNode->m_ObjectUBIndex);
			m_RenderNode->m_NumFramesDirty--;
		}
	}
}

void Triangle::RecordCommandBuffer(VulkanCommandBuffer * vulkanCommandBuffer)
{
	auto& driver = GetVulkanDriver();

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

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

	VulkanFramebuffer* vulkanFramebuffer = driver.RebuildCurrFramebuffer(m_VulkanRenderPass, driver.GetSwapChainCurrImageView(), m_DepthImage->m_ImageView, driver.GetSwapChainWidth(), driver.GetSwapChainHeight());

	vulkanCommandBuffer->Begin();

	vulkanCommandBuffer->BeginRenderPass(m_VulkanRenderPass, vulkanFramebuffer, area, clearValues);

	vulkanCommandBuffer->SetViewport(viewport);

	vulkanCommandBuffer->SetScissor(area);

	vulkanCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline);

	vulkanCommandBuffer->BindVertexBuffer(0, m_Mesh->GetVertexBuffer());

	vulkanCommandBuffer->BindIndexBuffer(m_Mesh->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);

	vulkanCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipelineLayout, m_VulkanDescriptorSet);

	vulkanCommandBuffer->DrawIndexed(m_Mesh->GetIndexCount(), 1, 0, 0, 1);

	vulkanCommandBuffer->EndRenderPass();

	vulkanCommandBuffer->End();
}

void Triangle::PrepareResources()
{
	auto& driver = GetVulkanDriver();

	// Mesh
	{
		std::vector<VertexChannel> channels = { kVertexPosition ,kVertexColor, kVertexTexcoord };

		m_Mesh = new Mesh();
		m_Mesh->SetVertexChannels(channels);
		m_Mesh->LoadFromFile(global::AssetPath + "models/viking_room.obj");
		m_Mesh->UploadToGPU();
	}

	// Texture
	{
		uint32_t width = 0, height = 0, dataSize = 0;
		std::vector<char> imageData = GetImageData(global::AssetPath + "textures/viking_room.png", 4, &width, &height, nullptr, &dataSize);
		if (imageData.size() == 0) {
			assert(false);
		}

		m_Image = driver.CreateVulkanImage(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

		driver.UploadVulkanImage(m_Image, imageData.data(), dataSize);
	}
	
	// RenderNode
	m_RenderNode = new RenderNode();
	m_RenderNode->m_ObjectUBIndex = 0;
	m_RenderNode->m_Mesh = m_Mesh;
	auto world = glm::mat4(1.0f);
	world = glm::rotate(glm::mat4(1.0f), -1.57f, glm::vec3(1.0f, 0.0f, 0.0f)) * world;
	world = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f)) * world;
	world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.3f)) * world;
	m_RenderNode->m_World = world;
}

void Triangle::PrepareDescriptorSet()
{
	auto& driver = GetVulkanDriver();

	DSLBindings bindings(3);
	bindings[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT };
	bindings[1] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT };
	bindings[2] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT };
	m_VulkanDescriptorSetLayout = driver.CreateVulkanDescriptorSetLayout(bindings);

	DPSizes sizes(2);
	sizes[0] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 };
	sizes[1] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 };
	m_VulkanDescriptorPool = driver.CreateVulkanDescriptorPool(1, sizes);

	m_VulkanDescriptorSet = m_VulkanDescriptorPool->AllocateDescriptorSet(m_VulkanDescriptorSetLayout);

	DesUpdateInfos infos(3);
	infos[0].binding = 0;
	infos[0].info.buffer = { driver.GetCurrPassUniformBuffer()->m_Buffer,0,sizeof(PassUniform) };
	infos[1].binding = 1;
	infos[1].info.buffer = { driver.GetCurrObjectUniformBuffer()->m_Buffer,0,sizeof(ObjectUniform) };
	infos[2].binding = 2;
	infos[2].info.image = { m_Image->m_Sampler,m_Image->m_ImageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	driver.UpdateDescriptorSets(m_VulkanDescriptorSet, infos);
}

void Triangle::CreatePipeline()
{
	auto& driver = GetVulkanDriver();

	m_VulkanRenderPass = driver.CreateVulkanRenderPass(driver.GetSwapChainFormat(), m_DepthFormat);

	// std::make_unique 需要C++14的支持，这里使用构造函数更保险
	std::shared_ptr<VulkanShaderModule> vulkanShaderModuleVert = std::shared_ptr<VulkanShaderModule>(driver.CreateVulkanShaderModule(global::AssetPath + "shaders/shader.vert.spv"));
	std::shared_ptr<VulkanShaderModule> vulkanShaderModuleFrag = std::shared_ptr<VulkanShaderModule>(driver.CreateVulkanShaderModule(global::AssetPath + "shaders/shader.frag.spv"));

	PipelineCI pipelineCI;

	pipelineCI.shaderStage.vertShaderModule = vulkanShaderModuleVert;
	pipelineCI.shaderStage.fragShaderModule = vulkanShaderModuleFrag;

	pipelineCI.vertexInputState.formats = m_Mesh->GetVertexFormats();

	pipelineCI.dynamicState.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	pipelineCI.dynamicState.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

	m_VulkanPipelineLayout = driver.CreateVulkanPipelineLayout(m_VulkanDescriptorSetLayout);

	pipelineCI.Configure(m_VulkanPipelineLayout, m_VulkanRenderPass);

	m_VulkanPipeline = driver.CreateVulkanPipeline(pipelineCI);
}

// 入口

#ifdef _WIN32

#if USE_VISUAL_LEAK_DETECTOR
#include "vld.h"
#endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	global::windowInstance = hInstance;

	ConfigGlobalSettings();

	application = new Application(new Triangle());
	application->Init();
	application->Run();
	application->CleanUp();
	RELEASE(application);

	system("PAUSE");
	return 0;
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void android_main(android_app* state)
{
	androidApp = state;

	ConfigGlobalSettings();

	application = new Application(new Triangle());
	state->userData = application;
	state->onAppCmd = HandleAppCommand;
	state->onInputEvent = HandleAppInput;
	application->Run();
	application->CleanUp();
	RELEASE(application);
}

#endif