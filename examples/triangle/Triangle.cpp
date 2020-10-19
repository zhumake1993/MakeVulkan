#include "Triangle.h"

#include "Tools.h"

#include "DeviceProperties.h"

#include "VulkanDriver.h"

#include "VKBuffer.h"
#include "VKImage.h"
#include "VKSampler.h"

#include "DescriptorSetMgr.h"

#include "VKShaderModule.h"
#include "VKPipelineLayout.h"
#include "VKPipeline.h"

#include "VKRenderPass.h"

#include "VKCommandBuffer.h"

#include "Application.h"
#include "Camera.h"
#include "Mesh.h"
#include "RenderNode.h"
#include "Texture.h"

#include "Gui.h"
#include "TimeMgr.h"
#include "ProfilerMgr.h"
#include "GPUProfilerMgr.h"

void ConfigGlobalSettings() {

	auto& dp = GetDeviceProperties();

	// 添加单独的实例级层

	// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
	// Note that on Android this layer requires at least NDK r20
#if defined(_WIN32)
	// 目前我用的NDK r19
	dp.enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// 添加单独的实例级拓展

	// 这个暂时搁置
	//global::enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	dp.enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
	dp.enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	dp.enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

	dp.enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// wireframe
	//dp.enabledDeviceFeatures.fillModeNonSolid = VK_TRUE;
	//dp.enabledDeviceFeatures.wideLines = VK_TRUE;
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

	RELEASE(m_HomeTex);
	RELEASE(m_CrateTex);
	RELEASE(m_Sampler);

	RELEASE(m_HomeNode);
	RELEASE(m_CubeNode1);
	RELEASE(m_CubeNode2);

	RELEASE(m_TexPipeline);
	RELEASE(m_ColorPipeline);
	RELEASE(m_PipelineLayout);
	RELEASE(m_VKRenderPass);

	RELEASE(m_Camera);

	RELEASE(m_DepthImage);
}

void Triangle::Init()
{
	m_PassUniform.lightPos = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

	auto& driver = GetVulkanDriver();

	m_DepthFormat = driver.GetSupportedDepthFormat();

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

void Triangle::Tick()
{
	PROFILER(APP_Tick);

	auto deltaTime = GetTimeMgr().GetDeltaTime();

	auto& driver = GetVulkanDriver();

	// 每帧的逻辑

	m_Camera->Update(deltaTime);

	m_PassUniform.view = m_Camera->GetView();
	m_PassUniform.proj = m_Camera->GetProj();
	m_PassUniform.eyePos = glm::vec4(m_Camera->GetPosition(), 0.0f);

	m_CubeNode1->m_World = glm::rotate(glm::mat4(1.0f), deltaTime * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) * m_CubeNode1->m_World;
	m_CubeNode1->m_NumFramesDirty = FrameResourcesCount;

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
	PROFILER(APP_TickUI);

	// UI样例，供学习用
	//bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);

	auto& dp = GetDeviceProperties();

	auto& timeMgr = GetTimeMgr();
	float fps = timeMgr.GetFPS();

	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::Begin("MakeVulkan", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted(dp.deviceProperties.deviceName);
	ImGui::Text("%.2f ms/frame (%.2f fps)", (1000.0f / fps), fps);

	static float acTime = 0;
	static std::string cpuProfiler = "";
	static std::string gpuProfiler = "";
	acTime += timeMgr.GetDeltaTime();
	if (acTime > 1.0f) {
		auto& profilerMgr = GetProfilerMgr();
		cpuProfiler = profilerMgr.Resolve(timeMgr.GetFrameIndex() - 1).ToString();

		gpuProfiler = m_GPUProfilerMgr->GetLastFrameView().ToString();

		acTime = 0.0f;
	}

	if (ImGui::CollapsingHeader("CPU Profiler", ImGuiTreeNodeFlags_None)) {
		ImGui::TextUnformatted(cpuProfiler.c_str());
	}

	if (ImGui::CollapsingHeader("GPU Profiler", ImGuiTreeNodeFlags_None)) {
		ImGui::TextUnformatted(gpuProfiler.c_str());
	}

	ImGui::End();
}

void Triangle::RecordCommandBuffer(VKCommandBuffer * vkCommandBuffer)
{
	PROFILER(APP_RecordCommandBuffer);

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
	auto descriptorSetHome = descriptorSetMgr.GetDescriptorSet(m_DescriptorSetLayout);

	DesUpdateInfos infos(3);
	infos[0].binding = 0;
	infos[0].info.buffer = { GetCurrPassUniformBuffer()->buffer,0,sizeof(PassUniform) };
	infos[1].binding = 1;
	infos[1].info.buffer = { GetCurrObjectUniformBuffer()->buffer,0,VK_WHOLE_SIZE };
	infos[2].binding = 2;
	infos[2].info.image = { m_Sampler->sampler, m_HomeTex->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	descriptorSetMgr.UpdateDescriptorSet(descriptorSetHome, infos);

	//

	VKFramebuffer* vkFramebuffer = RebuildFramebuffer(m_VKRenderPass, driver.GetSwapChainCurrImageView(), m_DepthImage->view, driver.GetSwapChainWidth(), driver.GetSwapChainHeight());

	vkCommandBuffer->Begin();

	m_GPUProfilerMgr->SetVKCommandBuffer(vkCommandBuffer);
	m_GPUProfilerMgr->Reset();

	m_GPUProfilerMgr->WriteTimeStamp("Render");

	// test!
	m_GPUProfilerMgr->WriteTimeStamp("child");
	m_GPUProfilerMgr->WriteTimeStamp("grandchild");
	m_GPUProfilerMgr->WriteTimeStamp("grandchild");
	m_GPUProfilerMgr->WriteTimeStamp("child");
	m_GPUProfilerMgr->WriteTimeStamp("sibling");
	m_GPUProfilerMgr->WriteTimeStamp("sibling");

	vkCommandBuffer->BeginRenderPass(m_VKRenderPass, vkFramebuffer, area, clearValues);

	vkCommandBuffer->SetViewport(viewport);

	vkCommandBuffer->SetScissor(area);

	vkCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_TexPipeline);

	// home
	vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, descriptorSetHome, 0 * GetUBODynamicAlignment());
	vkCommandBuffer->BindVertexBuffer(0, m_HomeNode->GetVertexBuffer());
	vkCommandBuffer->BindIndexBuffer(m_HomeNode->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
	vkCommandBuffer->DrawIndexed(m_HomeNode->GetIndexCount(), 1, 0, 0, 1);

	vkCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_ColorPipeline);

	auto descriptorSetCube = descriptorSetMgr.GetDescriptorSet(m_DescriptorSetLayout);

	DesUpdateInfos infos2(3);
	infos2[0].binding = 0;
	infos2[0].info.buffer = { GetCurrPassUniformBuffer()->buffer,0,sizeof(PassUniform) };
	infos2[1].binding = 1;
	infos2[1].info.buffer = { GetCurrObjectUniformBuffer()->buffer,0,VK_WHOLE_SIZE };
	infos2[2].binding = 2;
	infos2[2].info.image = { m_Sampler->sampler, m_CrateTex->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	descriptorSetMgr.UpdateDescriptorSet(descriptorSetCube, infos2);

	// cube1
	vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, descriptorSetCube, 1 * GetUBODynamicAlignment());
	vkCommandBuffer->BindVertexBuffer(0, m_CubeNode1->GetVertexBuffer());
	vkCommandBuffer->BindIndexBuffer(m_CubeNode1->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
	vkCommandBuffer->DrawIndexed(m_CubeNode1->GetIndexCount(), 1, 0, 0, 1);

	// cube2
	vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, descriptorSetCube, 2 * GetUBODynamicAlignment());
	vkCommandBuffer->BindVertexBuffer(0, m_CubeNode2->GetVertexBuffer());
	vkCommandBuffer->BindIndexBuffer(m_CubeNode2->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
	vkCommandBuffer->DrawIndexed(m_CubeNode2->GetIndexCount(), 1, 0, 0, 1);

	// todo
	m_Imgui->RecordCommandBuffer(vkCommandBuffer);

	vkCommandBuffer->EndRenderPass();

	m_GPUProfilerMgr->WriteTimeStamp("Render");

	vkCommandBuffer->End();
}

void Triangle::PrepareResources()
{
	auto& driver = GetVulkanDriver();

	// Mesh
	{
		std::vector<VertexChannel> channels = { kVertexPosition ,kVertexNormal, kVertexColor, kVertexTexcoord };

		m_Home = new Mesh();
		m_Home->SetVertexChannels(channels);
		m_Home->LoadFromFile(global::AssetPath + "models/viking_room.obj");
		m_Home->UploadToGPU();

		m_Cube = new Mesh();
		m_Cube->SetVertexChannels(channels);
		m_Cube->LoadFromFile(global::AssetPath + "models/cube.obj");
		m_Cube->UploadToGPU();
	}

	// Texture
	m_HomeTex = new Texture();
	m_HomeTex->LoadFromFile(global::AssetPath + "textures/viking_room.png");

	m_CrateTex = new Texture();
	m_CrateTex->LoadFromFile(global::AssetPath + "textures/crate01_color_height_rgba.ktx");

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
	m_CubeNode1->m_World = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.3f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));

	m_CubeNode2 = new RenderNode();
	m_CubeNode2->m_ObjectUBIndex = 2;
	m_CubeNode2->m_Mesh = m_Cube;
	m_CubeNode2->m_World = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
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
	m_VKRenderPass = driver.CreateVKRenderPass(driver.GetSwapChainFormat(), m_DepthFormat);

	VKShaderModule* shaderModuleVert = driver.CreateVKShaderModule(global::AssetPath + "shaders/triangle/shader.vert.spv");
	VKShaderModule* shaderModuleFrag = driver.CreateVKShaderModule(global::AssetPath + "shaders/triangle/shader.frag.spv");

	VKShaderModule* simpleColorVert = driver.CreateVKShaderModule(global::AssetPath + "shaders/triangle/simpleColor.vert.spv");
	VKShaderModule* simpleColorFrag = driver.CreateVKShaderModule(global::AssetPath + "shaders/triangle/simpleColor.frag.spv");

	PipelineCI pipelineCI;
	pipelineCI.pipelineCreateInfo.layout = m_PipelineLayout->pipelineLayout;
	pipelineCI.pipelineCreateInfo.renderPass = m_VKRenderPass->renderPass;

	// tex

	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = shaderModuleVert->shaderModule;
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = shaderModuleFrag->shaderModule;
	pipelineCI.SetVertexInputState(m_Home->GetVertexFormats());

	m_TexPipeline = driver.CreateVKPipeline(pipelineCI);

	// color

	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = shaderModuleVert->shaderModule;
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = shaderModuleFrag->shaderModule;
	pipelineCI.SetVertexInputState(m_Cube->GetVertexFormats());

	m_ColorPipeline = driver.CreateVKPipeline(pipelineCI, m_TexPipeline);

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