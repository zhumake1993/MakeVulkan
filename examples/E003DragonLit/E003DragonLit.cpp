#include "E003DragonLit.h"
#include "Application.h"
#include "DeviceProperties.h"
#include "GfxTypes.h"
#include "GfxDevice.h"
#include "Settings.h"
#include "Tools.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "RenderNode.h"
#include "Camera.h"
#include "TimeManager.h"
#include "Imgui.h"
#include "ProfilerManager.h"
#include "GpuProgram.h"

MakeVulkan::MakeVulkan()
{
}

MakeVulkan::~MakeVulkan()
{
}

void MakeVulkan::ConfigDeviceProperties()
{
	auto& dp = GetDeviceProperties();

	// 添加InstanceLayer
#if defined(_WIN32)
	dp.enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// 添加InstanceExtension
	dp.enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
	dp.enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	dp.enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

	// 添加DeviceExtension
	dp.enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// 添加DeviceFeature
}

void MakeVulkan::Init()
{
	Example::Init();

	auto& device = GetGfxDevice();

	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, -4.0f));
	m_Camera->SetLens(glm::radians(60.0f), 1.0f * windowWidth / windowHeight, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera->SetSpeed(3.0f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera->SetSpeed(0.005f, 0.005f);
#endif

	m_PointLightTransform.Translate(0.0f, 0.0f, -2.0f);

	PrepareResources();
}

void MakeVulkan::Release()
{
	RELEASE(m_Camera);

	Example::Release();
}

void MakeVulkan::Update()
{
	Example::Update();

	PROFILER(Update);

	float deltaTime = m_TimeManager->GetDeltaTime();

	m_Camera->Update(deltaTime);

	m_UniformDataPerView.view = m_Camera->GetView();
	m_UniformDataPerView.proj = m_Camera->GetProj();
	m_UniformDataPerView.eyePos = glm::vec4(m_Camera->GetPosition(), 1.0f);

	m_UniformDataPerView.ambientLight = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);

	// 平行光
	m_UniformDataPerView.lights[0].strength = glm::vec3(0.2f, 0.0f, 0.0f);
	m_UniformDataPerView.lights[0].direction = glm::vec3(1.0f, 0.0f, 0.0f);

	// 点光
	m_UniformDataPerView.lights[1].strength = glm::vec3(0.0f, 0.2f, 0.0f);
	m_UniformDataPerView.lights[1].falloffStart = 0.1f;
	m_UniformDataPerView.lights[1].falloffEnd = 5.0f;
	m_UniformDataPerView.lights[1].position = m_PointLightTransform.GetPosition();

	m_PointLightTransform.Rotate(-deltaTime * 0.5f, 0.0f, 1.0f, 0.0f);

	// 聚光
	m_UniformDataPerView.lights[2].strength = glm::vec3(0.0f, 0.0f, 1.0f);
	m_UniformDataPerView.lights[2].falloffStart = 0.1f;
	m_UniformDataPerView.lights[2].falloffEnd = 5.0f;
	m_UniformDataPerView.lights[2].position = m_Camera->GetPosition();
	m_UniformDataPerView.lights[2].direction = glm::normalize(m_Camera->GetDirection());
	m_UniformDataPerView.lights[2].spotPower = 64.0f;

	// Imgui

	auto& dp = GetDeviceProperties();

	float fps = m_TimeManager->GetFPS();

	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::Begin("MakeVulkan", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted(dp.deviceProperties.deviceName);
	ImGui::Text("%.2f ms/frame (%.2f fps)", (1000.0f / fps), fps);

	static float acTime = 0;
	static mkString cpuProfiler = "";
	static mkString gpuProfiler = "";
	acTime += deltaTime;
	if (acTime > 1.0f)
	{
		auto& profilerMgr = GetProfilerManager();
		cpuProfiler = profilerMgr.Resolve(m_TimeManager->GetFrameIndex() - 1).ToString();
		gpuProfiler = GetGfxDevice().GetLastGPUTimeStamp();

		acTime = 0.0f;
	}

	if (ImGui::CollapsingHeader("CPU Profiler", ImGuiTreeNodeFlags_None))
	{
		ImGui::TextUnformatted(cpuProfiler.c_str());
	}

	if (ImGui::CollapsingHeader("GPU Profiler", ImGuiTreeNodeFlags_None))
	{
		ImGui::TextUnformatted(gpuProfiler.c_str());
	}

	ImGui::End();

	UpdateImgui();
}

void MakeVulkan::Draw()
{
	PROFILER(Draw);

	auto& device = GetGfxDevice();

	device.BeginCommandBuffer();

	device.ResetTimeStamp();

	BindGlobalUniformBuffer(&m_UniformDataGlobal, sizeof(UniformDataGlobal));
	BindPerViewUniformBuffer(&m_UniformDataPerView, sizeof(UniformDataPerView));

	Color clearColor(0, 0, 0, 0);
	DepthStencil clearDepthStencil(1.0, 0);
	Rect2D area(0, 0, windowWidth, windowHeight);
	Viewport viewport(0, 0, windowWidth, windowHeight, 0, 1);

	device.WriteTimeStamp("RenderPass");

	device.BeginRenderPass(area, clearColor, clearDepthStencil);

	device.SetViewport(viewport);
	device.SetScissor(area);

	SetShader(m_Shader);

	BindMaterial(m_Mat);
	DrawRenderNode(m_Node);

	DrawImgui();

	device.EndRenderPass();

	device.WriteTimeStamp("RenderPass");

	device.EndCommandBuffer();
}

void MakeVulkan::PrepareResources()
{
	// Mesh
	{
		m_Mesh = CreateMesh("Mesh");
		m_Mesh->LoadFromFile(AssetPath + "models/viking_room.obj");
		m_Mesh->UploadToGPU();
	}

	// Texture
	{
		m_Tex = CreateTexture("Tex");
		m_Tex->LoadFromFile(AssetPath + "textures/viking_room.png");
	}

	// Shader
	{
		m_Shader = CreateShader("Shader");
		m_Shader->LoadSPV(AssetPath + "shaders/E003DragonLit/DragonLit.vert.spv", AssetPath + "shaders/E003DragonLit/DragonLit.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerMaterial", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			uniform.valueParameters.emplace_back("DiffuseAlbedo", kShaderDataFloat4);
			uniform.valueParameters.emplace_back("FresnelR0", kShaderDataFloat3);
			uniform.valueParameters.emplace_back("Roughness", kShaderDataFloat1);
			uniform.valueParameters.emplace_back("MatTransform", kShaderDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		{
			GpuParameters::TextureParameter texture("BaseTexture", 1, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.textureParameters.push_back(texture);
		}
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", kShaderDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		parameters.SCParameters.emplace_back(0, kShaderDataInt1);
		parameters.SCParameters.emplace_back(1, kShaderDataInt1);
		parameters.SCParameters.emplace_back(2, kShaderDataInt1);
		m_Shader->CreateGpuProgram(parameters);

		RenderState renderState;

		m_Shader->SetRenderState(renderState);

		m_Shader->SetSCInt(0, 1);
		m_Shader->SetSCInt(1, 1);
		m_Shader->SetSCInt(2, 1);
	}

	// Material
	{
		m_Mat = CreateMaterial("Mat");
		m_Mat->SetShader(m_Shader);
		m_Mat->SetFloat4("DiffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_Mat->SetFloat3("FresnelR0", 0.3f, 0.3f, 0.3f);
		m_Mat->SetFloat("Roughness", 0.9f);
		m_Mat->SetTexture("BaseTexture", m_Tex);
	}

	// RenderNode
	{
		m_Node = CreateRenderNode("Node");
		m_Node->SetMesh(m_Mesh);
		m_Node->SetMaterial(m_Mat);
		m_Node->GetTransform().Scale(3, 3, 3).Rotate(-1.57f, 1.0f, 0.0f, 0.0f).Rotate(1.57f, 0.0f, 1.0f, 0.0f).Translate(0.0f, -1.0f, 0.0f);
	}
}

MAIN(MakeVulkan)