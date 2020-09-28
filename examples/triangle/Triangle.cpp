#include "Triangle.h"

#include "VulkanDriver.h"

#include "VulkanBuffer.h"
#include "VKImage.h"
#include "VKSampler.h"

#include "DescriptorSetMgr.h"

#include "VulkanShaderModule.h"
#include "VKPipelineLayout.h"
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
	//global::enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
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
	RELEASE(m_Home);
	RELEASE(m_Cube);

	RELEASE(m_Image);
	RELEASE(m_Sampler);

	RELEASE(m_HomeNode);
	RELEASE(m_CubeNode1);
	RELEASE(m_CubeNode2);

	RELEASE(m_TexPipeline);
	RELEASE(m_ColorPipeline);
	RELEASE(m_PipelineLayout);
	RELEASE(m_VulkanRenderPass);

	RELEASE(m_Camera);

	RELEASE(m_DepthImage);
}

void Triangle::Init()
{
	m_PassUniform.lightPos = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

	auto& driver = GetVulkanDriver();

	m_DepthFormat = driver.GetDepthFormat();

	VKImageCI(imageCI);
	imageCI.format = m_DepthFormat;
	imageCI.extent.width = global::windowWidth;
	imageCI.extent.height = global::windowHeight;
	imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	VKImageViewCI(imageViewCI);
	imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	m_DepthImage = driver.CreateVKImage(imageCI, imageViewCI);

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

void Triangle::Tick(float deltaTime)
{
	auto& driver = GetVulkanDriver();

	// 每帧的逻辑

	m_Camera->Update(deltaTime);

	m_PassUniform.view = m_Camera->GetView();
	m_PassUniform.proj = m_Camera->GetProj();
	m_PassUniform.eyePos = glm::vec4(m_Camera->GetPosition(), 0.0f);

	m_CubeNode1->m_World = glm::rotate(glm::mat4(1.0f), deltaTime * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * m_CubeNode1->m_World;
	m_CubeNode1->m_NumFramesDirty = global::frameResourcesCount;

	m_PassUniform.lightPos = glm::rotate(glm::mat4(1.0f), deltaTime * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * m_PassUniform.lightPos;

	{
		UpdatePassUniformBuffer(&m_PassUniform);
	}
	{
		if (m_HomeNode->m_NumFramesDirty > 0) {
			UpdateObjectUniformBuffer(&m_HomeNode->m_World, m_HomeNode->m_ObjectUBIndex);
			m_HomeNode->m_NumFramesDirty--;
		}

		if (m_CubeNode1->m_NumFramesDirty > 0) {
			UpdateObjectUniformBuffer(&m_CubeNode1->m_World, m_CubeNode1->m_ObjectUBIndex);
			m_CubeNode1->m_NumFramesDirty--;
		}

		if (m_CubeNode2->m_NumFramesDirty > 0) {
			UpdateObjectUniformBuffer(&m_CubeNode2->m_World, m_CubeNode2->m_ObjectUBIndex);
			m_CubeNode2->m_NumFramesDirty--;
		}
	}
}

void Triangle::TickUI()
{
	//ImGui::Text("Hello from another window!");
	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
	//ImGui::Text("Hello from another window!");

	// example
	//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//if (show_demo_window)
	//	ImGui::ShowDemoWindow(&show_demo_window);

	//// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	//{
	//	static float f = 0.0f;
	//	static int counter = 0;

	//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//	ImGui::Checkbox("Another Window", &show_another_window);

	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//		counter++;
	//	ImGui::SameLine();
	//	ImGui::Text("counter = %d", counter);

	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//	ImGui::End();
	//}

	//// 3. Show another simple window.
	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		show_another_window = false;
	//	ImGui::End();
	//}
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

	// DescriptorSet
	auto& descriptorSetMgr = driver.GetDescriptorSetMgr();
	auto descriptorSet = descriptorSetMgr.GetDescriptorSet(m_DescriptorSetLayout);

	DesUpdateInfos infos(3);
	infos[0].binding = 0;
	infos[0].info.buffer = { GetCurrPassUniformBuffer()->m_Buffer,0,sizeof(PassUniform) };
	infos[1].binding = 1;
	infos[1].info.buffer = { GetCurrObjectUniformBuffer()->m_Buffer,0,sizeof(ObjectUniform) };
	infos[2].binding = 2;
	infos[2].info.image = { m_Sampler->GetSampler(),m_Image->GetView(),VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	descriptorSetMgr.UpdateDescriptorSet(descriptorSet, infos);

	//

	VulkanFramebuffer* vulkanFramebuffer = RebuildFramebuffer(m_VulkanRenderPass, driver.GetSwapChainCurrImageView(), m_DepthImage->GetView(), driver.GetSwapChainWidth(), driver.GetSwapChainHeight());

	vulkanCommandBuffer->Begin();

	vulkanCommandBuffer->BeginRenderPass(m_VulkanRenderPass, vulkanFramebuffer, area, clearValues);

	vulkanCommandBuffer->SetViewport(viewport);

	vulkanCommandBuffer->SetScissor(area);

	vulkanCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_TexPipeline);

	// home
	vulkanCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, descriptorSet, 0 * sizeof(ObjectUniform));
	vulkanCommandBuffer->BindVertexBuffer(0, m_HomeNode->GetVertexBuffer());
	vulkanCommandBuffer->BindIndexBuffer(m_HomeNode->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
	vulkanCommandBuffer->DrawIndexed(m_HomeNode->GetIndexCount(), 1, 0, 0, 1);

	vulkanCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_ColorPipeline);

	// cube1
	vulkanCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, descriptorSet, 1 * sizeof(ObjectUniform));
	vulkanCommandBuffer->BindVertexBuffer(0, m_CubeNode1->GetVertexBuffer());
	vulkanCommandBuffer->BindIndexBuffer(m_CubeNode1->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
	vulkanCommandBuffer->DrawIndexed(m_CubeNode1->GetIndexCount(), 1, 0, 0, 1);

	// cube2
	vulkanCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, descriptorSet, 2 * sizeof(ObjectUniform));
	vulkanCommandBuffer->BindVertexBuffer(0, m_CubeNode2->GetVertexBuffer());
	vulkanCommandBuffer->BindIndexBuffer(m_CubeNode2->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
	vulkanCommandBuffer->DrawIndexed(m_CubeNode2->GetIndexCount(), 1, 0, 0, 1);

	// todo
	m_Imgui->RecordCommandBuffer(vulkanCommandBuffer);

	vulkanCommandBuffer->EndRenderPass();

	vulkanCommandBuffer->End();
}

void Triangle::PrepareResources()
{
	auto& driver = GetVulkanDriver();

	// Mesh
	{
		std::vector<VertexChannel> channels = { kVertexPosition ,kVertexNormal, kVertexTexcoord };

		m_Home = new Mesh();
		m_Home->SetVertexChannels(channels);
		m_Home->LoadFromFile(global::AssetPath + "models/viking_room.obj");
		m_Home->UploadToGPU();

		std::vector<VertexChannel> simpleChannels = { kVertexPosition ,kVertexColor };
		m_Cube = new Mesh();
		m_Cube->SetVertexChannels(simpleChannels);
		m_Cube->LoadFromGeo();
		m_Cube->UploadToGPU();
	}

	// Texture
	{
		uint32_t width = 0, height = 0, dataSize = 0;
		std::vector<char> imageData = GetImageData(global::AssetPath + "textures/viking_room.png", 4, &width, &height, nullptr, &dataSize);
		if (imageData.size() == 0) {
			assert(false);
		}

		VKImageCI(imageCI);
		imageCI.extent.width = width;
		imageCI.extent.height = height;
		VKImageViewCI(imageViewCI)
		m_Image = driver.CreateVKImage(imageCI, imageViewCI);

		driver.UploadVKImage(m_Image, imageData.data(), dataSize);
	}

	// Sampler
	{
		VKSamplerCI(samplerCI);
		m_Sampler = driver.CreateVKSampler(samplerCI);
	}
	
	// RenderNode
	m_HomeNode = new RenderNode();
	m_HomeNode->m_ObjectUBIndex = 0;
	m_HomeNode->m_Mesh = m_Home;
	auto world = glm::mat4(1.0f);
	world = glm::rotate(glm::mat4(1.0f), -1.57f, glm::vec3(1.0f, 0.0f, 0.0f)) * world;
	world = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f)) * world;
	world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.3f)) * world;
	m_HomeNode->m_World = world;

	m_CubeNode1 = new RenderNode();
	m_CubeNode1->m_ObjectUBIndex = 1;
	m_CubeNode1->m_Mesh = m_Cube;
	m_CubeNode1->m_World = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.3f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

	m_CubeNode2 = new RenderNode();
	m_CubeNode2->m_ObjectUBIndex = 2;
	m_CubeNode2->m_Mesh = m_Cube;
	m_CubeNode2->m_World = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
}

void Triangle::PrepareDescriptorSet()
{
	auto& driver = GetVulkanDriver();
	auto& descriptorSetMgr = driver.GetDescriptorSetMgr();

	DSLBindings bindings(3);
	bindings[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT };
	bindings[1] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT };
	bindings[2] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT };
	m_DescriptorSetLayout = descriptorSetMgr.CreateDescriptorSetLayout(bindings);
}

void Triangle::CreatePipeline()
{
	auto& driver = GetVulkanDriver();

	m_PipelineLayout = driver.CreateVKPipelineLayout(m_DescriptorSetLayout);
	m_VulkanRenderPass = driver.CreateVulkanRenderPass(driver.GetSwapChainFormat(), m_DepthFormat);

	VulkanShaderModule* shaderModuleVert = driver.CreateVulkanShaderModule(global::AssetPath + "shaders/triangle/shader.vert.spv");
	VulkanShaderModule* shaderModuleFrag = driver.CreateVulkanShaderModule(global::AssetPath + "shaders/triangle/shader.frag.spv");

	VulkanShaderModule* simpleColorVert = driver.CreateVulkanShaderModule(global::AssetPath + "shaders/triangle/simpleColor.vert.spv");
	VulkanShaderModule* simpleColorFrag = driver.CreateVulkanShaderModule(global::AssetPath + "shaders/triangle/simpleColor.frag.spv");

	PipelineCI pipelineCI;
	pipelineCI.pipelineCreateInfo.layout = m_PipelineLayout->GetLayout();
	pipelineCI.pipelineCreateInfo.renderPass = m_VulkanRenderPass->m_RenderPass;

	// tex

	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = shaderModuleVert->m_ShaderModule;
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = shaderModuleFrag->m_ShaderModule;
	pipelineCI.SetVertexInputState(m_Home->GetVertexFormats());

	m_TexPipeline = driver.CreateVulkanPipeline(pipelineCI);

	// color

	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = simpleColorVert->m_ShaderModule;
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = simpleColorFrag->m_ShaderModule;
	pipelineCI.SetVertexInputState(m_Cube->GetVertexFormats());

	m_ColorPipeline = driver.CreateVulkanPipeline(pipelineCI);

	RELEASE(shaderModuleVert);
	RELEASE(shaderModuleFrag);
	RELEASE(simpleColorVert);
	RELEASE(simpleColorFrag);
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

	SetWindowText(global::windowHandle, "Triangle");

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