#include "E002Texture.h"
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

	// Camera
	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetLens(glm::radians(60.0f), 1.0f * windowWidth / windowHeight, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera->SetSpeed(3.0f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera->SetSpeed(0.005f, 0.005f);
#endif

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

	m_Node->GetTransform().Rotate(-deltaTime * 0.5f, 0.0f, 1.0f, 0.0f);

	// Imgui

	auto& dp = GetDeviceProperties();

	float fps = m_TimeManager->GetFPS();

	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::Begin("MakeVulkan", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted(dp.deviceProperties.deviceName);
	ImGui::Text("%.2f ms/frame (%.2f fps)", (1000.0f / fps), fps);

	static float acTime = 0;
	static std::string cpuProfiler = "";
	static std::string gpuProfiler = "";
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
		m_Mesh->SetVertexChannels({ kVertexPosition, kVertexTexcoord0 });
		m_Mesh->LoadFromFile(AssetPath + "models/cube.obj");
		m_Mesh->UploadToGPU();
	}

	// Texture
	{
		m_Tex = CreateTexture("Tex");
		m_Tex->LoadFromFile(AssetPath + "textures/metalplate_nomips_rgba.ktx"); // 没有mipmap
	}

	// Shader
	{
		m_Shader = CreateShader("Shader");
		m_Shader->LoadSPV(AssetPath + "shaders/E002Texture/Tex.vert.spv", AssetPath + "shaders/E002Texture/Tex.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::TextureParameter texture("BaseTexture", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.textureParameters.push_back(texture);
		}
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", kShaderDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		m_Shader->CreateGpuProgram(parameters);

		RenderState renderState;

		m_Shader->SetRenderState(renderState);
	}

	// Material
	{
		m_Mat = CreateMaterial("Mat");
		m_Mat->SetShader(m_Shader);
		m_Mat->SetTexture("BaseTexture", m_Tex);
	}

	// RenderNode
	{
		m_Node = CreateRenderNode("Node");
		m_Node->SetMesh(m_Mesh);
		m_Node->SetMaterial(m_Mat);
		m_Node->GetTransform().Scale(0.1f, 0.1f, 0.1f);
	}
}

MAIN(MakeVulkan)