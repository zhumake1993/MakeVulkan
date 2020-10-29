#include "Triangle.h"

#include "Tools.h"

#include "DeviceProperties.h"

#include "VulkanDriver.h"

#include "VKBuffer.h"
#include "VKImage.h"
#include "VKSampler.h"

#include "DescriptorSetMgr.h"

#include "VKPipeline.h"

#include "VKRenderPass.h"

#include "VKCommandBuffer.h"

#include "Application.h"
#include "Camera.h"
#include "Mesh.h"
#include "RenderNode.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"

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
	RELEASE(m_Sampler);

	RELEASE(m_HomeMatPipeline);
	RELEASE(m_Crate01MatPipeline);
	RELEASE(m_Crate02MatPipeline);
	RELEASE(m_SimpleColorMatPipeline);

	RELEASE(m_VKRenderPass);

	RELEASE(m_Camera);
}

void Triangle::Init()
{
	auto& driver = GetVulkanDriver();

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
	m_PassUniform.eyePos = glm::vec4(m_Camera->GetPosition(), 1.0f);

	m_CubeNode1->GetTransform().Rotate(deltaTime * 0.5f, 0.0f, 1.0f, 0.0f);
	m_CubeNode1->SetDirty();

	m_SphereNode->GetTransform().Rotate(-deltaTime * 0.5f, 0.0f, 1.0f, 0.0f);
	m_SphereNode->SetDirty();

	m_PassUniform.ambientLight = glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);

	m_PassUniform.lights[0].strength = glm::vec3(0.9f, 0.9f, 0.9f);
	m_PassUniform.lights[0].direction = glm::vec3(1.0f, 0.0f, 0.0f);

	m_PassUniform.lights[1].strength = glm::vec3(0.3f, 0.3f, 0.3f);
	m_PassUniform.lights[1].falloffStart = 0.1f;
	m_PassUniform.lights[1].falloffEnd = 1.0f;
	m_PassUniform.lights[1].position = m_CubeNode1->GetTransform().GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

		gpuProfiler = GetVulkanDriver().GetLastFrameGPUProfilerResult();

		acTime = 0.0f;
	}

	if (ImGui::CollapsingHeader("Test", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderFloat("Slider", &m_Temp, 0, 1);
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

	//

	VKFramebuffer* vkFramebuffer = driver.RebuildFramebuffer(m_VKRenderPass);

	// test!
	vkCommandBuffer->WriteTimeStamp("child");
	vkCommandBuffer->WriteTimeStamp("grandchild");
	vkCommandBuffer->WriteTimeStamp("grandchild");
	vkCommandBuffer->WriteTimeStamp("child");
	vkCommandBuffer->WriteTimeStamp("sibling");
	vkCommandBuffer->WriteTimeStamp("sibling");

	vkCommandBuffer->BeginRenderPass(m_VKRenderPass, vkFramebuffer, area, clearValues);

	vkCommandBuffer->SetViewport(viewport);

	vkCommandBuffer->SetScissor(area);

	//
	auto passBuffer = driver.GetUniformBuffer("PassUniform");
	auto objectUniform = driver.GetUniformBuffer("ObjectUniform");

	auto dsPerDrawcall = driver.GetDescriptorSet(m_DSLPassUniform);
	DesUpdateInfos infosPerDrawcall(1);
	infosPerDrawcall[0].binding = 0;
	infosPerDrawcall[0].info.buffer = { passBuffer->buffer,0,sizeof(PassUniform) };
	driver.UpdateDescriptorSet(dsPerDrawcall, infosPerDrawcall);
	vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_HomeMatPipeline->pipelineLayout, 0, dsPerDrawcall);

	auto dsObjectDUB = driver.GetDescriptorSet(m_DSLObjectDUB);
	DesUpdateInfos infosObjectDUB(1);
	infosObjectDUB[0].binding = 0;
	infosObjectDUB[0].info.buffer = { objectUniform->buffer,0,VK_WHOLE_SIZE };
	driver.UpdateDescriptorSet(dsObjectDUB, infosObjectDUB);

	// m_HomeMat
	{
		vkCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_HomeMatPipeline);

		auto descriptorSet = driver.GetDescriptorSet(m_HomeMat->GetVkDescriptorSetLayout());
		DesUpdateInfos infos(2);
		infos[0].binding = 0;
		infos[0].info.buffer = { driver.GetUniformBuffer("m_HomeMat")->buffer,0,m_HomeMat->GetUniformDataSize() };
		infos[1].binding = 1;
		infos[1].info.image = { m_Sampler->sampler, m_HomeMat->GetTextures()[0]->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		driver.UpdateDescriptorSet(descriptorSet, infos);
		vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_HomeMatPipeline->pipelineLayout, 2, descriptorSet);

		// drawcall
		{
			vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_HomeMatPipeline->pipelineLayout, 1, dsObjectDUB, m_HomeNode->GetDUBIndex() * m_ObjectUBAlignment);
			vkCommandBuffer->BindVertexBuffer(0, m_HomeNode->GetVertexBuffer());
			vkCommandBuffer->BindIndexBuffer(m_HomeNode->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
			vkCommandBuffer->DrawIndexed(m_HomeNode->GetIndexCount(), 1, 0, 0, 1);
		}
	}

	// m_Crate01Mat
	{
		vkCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_Crate01MatPipeline);

		auto descriptorSet = driver.GetDescriptorSet(m_Crate01Mat->GetVkDescriptorSetLayout());
		DesUpdateInfos infos(2);
		infos[0].binding = 0;
		infos[0].info.buffer = { driver.GetUniformBuffer("m_Crate01Mat")->buffer,0,m_Crate01Mat->GetUniformDataSize() };
		infos[1].binding = 1;
		infos[1].info.image = { m_Sampler->sampler, m_Crate01Mat->GetTextures()[0]->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		driver.UpdateDescriptorSet(descriptorSet, infos);
		vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_Crate01MatPipeline->pipelineLayout, 2, descriptorSet);

		// drawcall
		{
			vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_Crate01MatPipeline->pipelineLayout, 1, dsObjectDUB, m_CubeNode1->GetDUBIndex() * m_ObjectUBAlignment);
			vkCommandBuffer->BindVertexBuffer(0, m_CubeNode1->GetVertexBuffer());
			vkCommandBuffer->BindIndexBuffer(m_CubeNode1->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
			vkCommandBuffer->DrawIndexed(m_CubeNode1->GetIndexCount(), 1, 0, 0, 1);
		}
	}

	// m_Crate02Mat
	{
		vkCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_Crate02MatPipeline);

		auto descriptorSet = driver.GetDescriptorSet(m_Crate02Mat->GetVkDescriptorSetLayout());
		DesUpdateInfos infos(2);
		infos[0].binding = 0;
		infos[0].info.buffer = { driver.GetUniformBuffer("m_Crate01Mat")->buffer,0,m_Crate02Mat->GetUniformDataSize() };
		infos[1].binding = 1;
		infos[1].info.image = { m_Sampler->sampler, m_Crate02Mat->GetTextures()[0]->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		driver.UpdateDescriptorSet(descriptorSet, infos);
		vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_Crate02MatPipeline->pipelineLayout, 2, descriptorSet);

		// drawcall
		{
			vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_Crate02MatPipeline->pipelineLayout, 1, dsObjectDUB, m_SphereNode->GetDUBIndex() * m_ObjectUBAlignment);
			vkCommandBuffer->BindVertexBuffer(0, m_SphereNode->GetVertexBuffer());
			vkCommandBuffer->BindIndexBuffer(m_SphereNode->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
			vkCommandBuffer->DrawIndexed(m_SphereNode->GetIndexCount(), 1, 0, 0, 1);
		}
	}

	// m_SimpleColorMat
	{
		vkCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_SimpleColorMatPipeline);

		// drawcall
		{
			vkCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_SimpleColorMatPipeline->pipelineLayout, 1, dsObjectDUB, m_ColorCubeNode->GetDUBIndex() * m_ObjectUBAlignment);
			vkCommandBuffer->BindVertexBuffer(0, m_ColorCubeNode->GetVertexBuffer());
			vkCommandBuffer->BindIndexBuffer(m_ColorCubeNode->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
			vkCommandBuffer->DrawIndexed(m_ColorCubeNode->GetIndexCount(), 1, 0, 0, 1);
		}
	}

	// todo
	m_Imgui->RecordCommandBuffer(vkCommandBuffer);

	vkCommandBuffer->EndRenderPass();

	vkCommandBuffer->WriteTimeStamp("Render");

	vkCommandBuffer->End();
}

void Triangle::PrepareResources()
{
	auto& driver = GetVulkanDriver();

	// Mesh
	{
		m_Home = CreateMesh();
		m_Home->LoadFromFile(global::AssetPath + "models/viking_room.obj");
		m_Home->UploadToGPU();

		m_Cube = CreateMesh();
		m_Cube->LoadFromFile(global::AssetPath + "models/cube.obj");
		m_Cube->UploadToGPU();

		m_SimpleCube = CreateMesh();
		m_SimpleCube->SetVertexChannels({ kVertexPosition, kVertexColor });
		std::vector<float> vertices = {
			-1.0f, -1.0f,  1.0f , 1.0f, 0.0f, 0.0f  ,
			1.0f, -1.0f,  1.0f , 0.0f, 1.0f, 0.0f  ,
			1.0f,  1.0f,  1.0f , 0.0f, 0.0f, 1.0f  ,
			-1.0f,  1.0f,  1.0f , 0.0f, 0.0f, 0.0f  ,
			-1.0f, -1.0f, -1.0f , 1.0f, 0.0f, 0.0f  ,
			1.0f, -1.0f, -1.0f , 0.0f, 1.0f, 0.0f  ,
			1.0f,  1.0f, -1.0f , 0.0f, 0.0f, 1.0f  ,
			-1.0f,  1.0f, -1.0f , 0.0f, 0.0f, 0.0f  ,
		};
		std::vector<uint32_t> indices = {
			0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 4,5,1, 1,0,4, 3,2,6, 6,7,3,
		};
		m_SimpleCube->SetVertices(vertices);
		m_SimpleCube->SetIndices(indices);
		m_SimpleCube->UploadToGPU();

		m_Sphere = CreateMesh();
		m_Sphere->LoadFromFile(global::AssetPath + "models/sphere.obj");
		m_Sphere->UploadToGPU();
	}

	// Texture
	{
		m_HomeTex = CreateTexture();
		m_HomeTex->LoadFromFile(global::AssetPath + "textures/viking_room.png");

		m_Crate01Tex = CreateTexture();
		m_Crate01Tex->LoadFromFile(global::AssetPath + "textures/crate01_color_height_rgba.ktx");

		m_Crate02Tex = CreateTexture();
		m_Crate02Tex->LoadFromFile(global::AssetPath + "textures/crate02_color_height_rgba.ktx");
	}

	// Shader
	{
		m_Shader = CreateShader();
		m_Shader->LoadVertSPV(global::AssetPath + "shaders/triangle/shader.vert.spv");
		m_Shader->LoadFragSPV(global::AssetPath + "shaders/triangle/shader.frag.spv");
		m_Shader->SetUniformLayout({ 
			{float4,"diffuseAlbedo"},
			{float3,"fresnelR0"},
			{float1,"roughness"},
			{float4x4,"matTransform"} 
		});
		m_Shader->SetTextureLayout({ "baseTexture" });

		m_SimpleShader = CreateShader();
		m_SimpleShader->LoadVertSPV(global::AssetPath + "shaders/triangle/simpleColor.vert.spv");
		m_SimpleShader->LoadFragSPV(global::AssetPath + "shaders/triangle/simpleColor.frag.spv");
	}

	// Material
	{
		m_HomeMat = CreateMaterial("m_HomeMat");
		m_HomeMat->SetShader(m_Shader);
		m_HomeMat->SetFloat4("diffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_HomeMat->SetFloat3("fresnelR0", 0.3f, 0.3f, 0.3f);
		m_HomeMat->SetFloat("roughness", 0.3f);
		m_HomeMat->SetTextures("baseTexture", m_HomeTex);

		m_Crate01Mat = CreateMaterial("m_Crate01Mat");
		m_Crate01Mat->SetShader(m_Shader);
		m_Crate01Mat->SetFloat4("diffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_Crate01Mat->SetFloat3("fresnelR0", 0.3f, 0.3f, 0.3f);
		m_Crate01Mat->SetFloat("roughness", 0.3f);
		m_Crate01Mat->SetTextures("baseTexture", m_Crate01Tex);

		m_Crate02Mat = CreateMaterial("m_Crate02Mat");
		m_Crate02Mat->SetShader(m_Shader);
		m_Crate02Mat->SetFloat4("diffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_Crate02Mat->SetFloat3("fresnelR0", 0.3f, 0.3f, 0.3f);
		m_Crate02Mat->SetFloat("roughness", 0.3f);
		m_Crate02Mat->SetTextures("baseTexture", m_Crate02Tex);

		m_SimpleColorMat = CreateMaterial("m_SimpleColorMat");
		m_SimpleColorMat->SetShader(m_SimpleShader);
	}

	// Sampler
	{
		VKSamplerCI(samplerCI);
		m_Sampler = driver.CreateVKSampler(samplerCI);
	}
	
	// RenderNode
	{
		m_HomeNode = CreateRenderNode();
		m_HomeNode->SetMesh(m_Home);
		m_HomeNode->GetTransform().Rotate(-1.57f, 1.0f, 0.0f, 0.0f).Rotate(1.57f, 0.0f, 1.0f, 0.0f).Translate(0.0f, -0.3f, 0.3f);

		m_CubeNode1 = CreateRenderNode();
		m_CubeNode1->SetMesh(m_Cube);
		m_CubeNode1->GetTransform().Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, 0.3f, 0.3f);

		m_SphereNode = CreateRenderNode();
		m_SphereNode->SetMesh(m_Sphere);
		m_SphereNode->GetTransform().Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, 0.3f, 0.0f);

		m_ColorCubeNode = CreateRenderNode();
		m_ColorCubeNode->SetMesh(m_SimpleCube);
		m_ColorCubeNode->GetTransform().Scale(0.1f, 0.1f, 0.1f).Translate(0.0f, 0.6f, 0.0f);
	}
}

void Triangle::CreatePipeline()
{
	auto& driver = GetVulkanDriver();

	// 目前手动构造pso

	PipelineCI pipelineCI;

	m_VKRenderPass = driver.CreateVKRenderPass(driver.GetSwapChainFormat());
	pipelineCI.pipelineCreateInfo.renderPass = m_VKRenderPass->renderPass;

	// m_HomeMat
	pipelineCI.pipelineCreateInfo.layout = driver.CreateVkPipelineLayout({ m_DSLPassUniform, m_DSLObjectDUB,m_HomeMat->GetVkDescriptorSetLayout() });
	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = m_HomeMat->GetShader()->GetVkShaderModuleVert();
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = m_HomeMat->GetShader()->GetVkShaderModuleFrag();
	pipelineCI.SetVertexInputState(m_Home->GetVertexDescription());
	m_HomeMatPipeline = driver.CreateVKPipeline(pipelineCI);

	// m_Crate01Mat
	pipelineCI.pipelineCreateInfo.layout = driver.CreateVkPipelineLayout({ m_DSLPassUniform, m_DSLObjectDUB,m_Crate01Mat->GetVkDescriptorSetLayout() });
	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = m_Crate01Mat->GetShader()->GetVkShaderModuleVert();
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = m_Crate01Mat->GetShader()->GetVkShaderModuleFrag();
	pipelineCI.SetVertexInputState(m_Home->GetVertexDescription());
	m_Crate01MatPipeline = driver.CreateVKPipeline(pipelineCI);

	// m_Crate02Mat
	pipelineCI.pipelineCreateInfo.layout = driver.CreateVkPipelineLayout({ m_DSLPassUniform, m_DSLObjectDUB,m_Crate02Mat->GetVkDescriptorSetLayout() });
	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = m_Crate02Mat->GetShader()->GetVkShaderModuleVert();
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = m_Crate02Mat->GetShader()->GetVkShaderModuleFrag();
	pipelineCI.SetVertexInputState(m_Home->GetVertexDescription());
	m_Crate02MatPipeline = driver.CreateVKPipeline(pipelineCI);

	// m_SimpleColorMat
	pipelineCI.pipelineCreateInfo.layout = driver.CreateVkPipelineLayout({ m_DSLPassUniform, m_DSLObjectDUB,m_SimpleColorMat->GetVkDescriptorSetLayout() });
	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = m_SimpleColorMat->GetShader()->GetVkShaderModuleVert();
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = m_SimpleColorMat->GetShader()->GetVkShaderModuleFrag();
	pipelineCI.SetVertexInputState(m_SimpleCube->GetVertexDescription());
	m_SimpleColorMatPipeline = driver.CreateVKPipeline(pipelineCI);
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