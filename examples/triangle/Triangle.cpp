#include "Triangle.h"

#include "Tools.h"

#include "DeviceProperties.h"

#include "VulkanDriver.h"

#include "VKBuffer.h"
#include "VKImage.h"

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
	dp.enabledDeviceFeatures.samplerAnisotropy = VK_TRUE;
}

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void Triangle::CleanUp()
{
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

void Triangle::RecordCommandBuffer(VKCommandBuffer * cb)
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
	//cb->WriteTimeStamp("child");
	//cb->WriteTimeStamp("grandchild");
	//cb->WriteTimeStamp("grandchild");
	//cb->WriteTimeStamp("child");
	//cb->WriteTimeStamp("sibling");
	//cb->WriteTimeStamp("sibling");

	cb->BeginRenderPass(m_VKRenderPass, vkFramebuffer, area, clearValues);

	cb->SetViewport(viewport);

	cb->SetScissor(area);

	DrawRenderNode(cb, m_HomeNode);
	DrawRenderNode(cb, m_CubeNode1);
	DrawRenderNode(cb, m_SphereNode);
	DrawRenderNode(cb, m_QuadNode);
	DrawRenderNode(cb, m_ColorCubeNode);

	// todo
	m_Imgui->RecordCommandBuffer(cb);

	cb->EndRenderPass();

	cb->WriteTimeStamp("Render");

	cb->End();
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

		m_Quad = CreateMesh();
		m_Quad->SetVertexChannels({ kVertexPosition, kVertexNormal, kVertexTexcoord });
		m_Quad->SetVertices({
			  1.0f,  1.0f, 0.0f , 0.0f, 0.0f, -1.0f , 1.0f, 1.0f ,
			 -1.0f,  1.0f, 0.0f , 0.0f, 0.0f, -1.0f , 0.0f, 1.0f ,
			 -1.0f, -1.0f, 0.0f , 0.0f, 0.0f, -1.0f , 0.0f, 0.0f ,
			  1.0f, -1.0f, 0.0f , 0.0f, 0.0f, -1.0f , 1.0f, 0.0f ,
			});
		m_Quad->SetIndices({
			0,2,1, 0,3,2
			});
		m_Quad->UploadToGPU();
	}

	// Texture
	{
		m_HomeTex = CreateTexture();
		m_HomeTex->LoadFromFile(global::AssetPath + "textures/viking_room.png");

		m_Crate01Tex = CreateTexture();
		m_Crate01Tex->LoadFromFile(global::AssetPath + "textures/crate01_color_height_rgba.ktx");

		m_Crate02Tex = CreateTexture();
		m_Crate02Tex->LoadFromFile(global::AssetPath + "textures/crate02_color_height_rgba.ktx");

		m_Metalplate = CreateTexture();
		m_Metalplate->LoadFromFile(global::AssetPath + "textures/metalplate_nomips_rgba.ktx");
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
		m_Shader->AddSpecializationConstant(0, 1);
		m_Shader->AddSpecializationConstant(1, 1);
		m_Shader->AddSpecializationConstant(2, 0);

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
		CreateVKPipeline(m_HomeMat, m_Home->GetVertexDescription());

		m_Crate01Mat = CreateMaterial("m_Crate01Mat");
		m_Crate01Mat->SetShader(m_Shader);
		m_Crate01Mat->SetFloat4("diffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_Crate01Mat->SetFloat3("fresnelR0", 0.3f, 0.3f, 0.3f);
		m_Crate01Mat->SetFloat("roughness", 0.3f);
		m_Crate01Mat->SetTextures("baseTexture", m_Crate01Tex);
		CreateVKPipeline(m_Crate01Mat, m_Home->GetVertexDescription());

		m_Crate02Mat = CreateMaterial("m_Crate02Mat");
		m_Crate02Mat->SetShader(m_Shader);
		m_Crate02Mat->SetFloat4("diffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_Crate02Mat->SetFloat3("fresnelR0", 0.3f, 0.3f, 0.3f);
		m_Crate02Mat->SetFloat("roughness", 0.3f);
		m_Crate02Mat->SetTextures("baseTexture", m_Crate02Tex);
		CreateVKPipeline(m_Crate02Mat, m_Home->GetVertexDescription());

		m_SimpleColorMat = CreateMaterial("m_SimpleColorMat");
		m_SimpleColorMat->SetShader(m_SimpleShader);
		CreateVKPipeline(m_SimpleColorMat, m_SimpleCube->GetVertexDescription());

		m_MetalplateMat = CreateMaterial("m_MetalplateMat");
		m_MetalplateMat->SetShader(m_Shader);
		m_MetalplateMat->SetFloat4("diffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_MetalplateMat->SetFloat3("fresnelR0", 0.3f, 0.3f, 0.3f);
		m_MetalplateMat->SetFloat("roughness", 0.3f);
		m_MetalplateMat->SetTextures("baseTexture", m_Metalplate);
		CreateVKPipeline(m_MetalplateMat, m_Quad->GetVertexDescription());
	}
	
	// RenderNode
	{
		m_HomeNode = CreateRenderNode();
		m_HomeNode->SetMesh(m_Home);
		m_HomeNode->SetMaterial(m_HomeMat);
		m_HomeNode->GetTransform().Rotate(-1.57f, 1.0f, 0.0f, 0.0f).Rotate(1.57f, 0.0f, 1.0f, 0.0f).Translate(0.0f, -0.3f, 0.3f);

		m_CubeNode1 = CreateRenderNode();
		m_CubeNode1->SetMesh(m_Cube);
		m_CubeNode1->SetMaterial(m_Crate01Mat);
		m_CubeNode1->GetTransform().Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, 0.3f, 0.3f);

		m_SphereNode = CreateRenderNode();
		m_SphereNode->SetMesh(m_Sphere);
		m_SphereNode->SetMaterial(m_Crate02Mat);
		m_SphereNode->GetTransform().Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, 0.3f, 0.0f);

		m_ColorCubeNode = CreateRenderNode();
		m_ColorCubeNode->SetMesh(m_SimpleCube);
		m_ColorCubeNode->SetMaterial(m_SimpleColorMat);
		m_ColorCubeNode->GetTransform().Scale(0.1f, 0.1f, 0.1f).Translate(0.0f, 0.6f, 0.0f);

		m_QuadNode = CreateRenderNode();
		m_QuadNode->SetMesh(m_Quad);
		m_QuadNode->SetMaterial(m_MetalplateMat);
		m_QuadNode->GetTransform().Scale(0.1f, 0.1f, 0.1f).Rotate(0.57f, 0.0f, 1.0f, 0.0f).Translate(0.0f, -0.6f, 0.0f);
	}
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