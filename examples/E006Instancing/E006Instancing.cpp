#include "E006Instancing.h"
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
	m_Camera->LookAt(glm::vec3(1.5f, 1.5f, -5.0f), glm::vec3(1.5f, 1.5f, 0.0f));
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

	DrawInstanced(m_Mesh, m_Shader, m_InstanceData, m_InstanceSize, 100);

	DrawImgui();

	device.EndRenderPass();

	device.WriteTimeStamp("RenderPass");

	device.EndCommandBuffer();
}

void MakeVulkan::PrepareResources()
{
	// Mesh
	{
		// 自定义顶点buffer的结构和数据
		m_Mesh = CreateMesh("Mesh");
		m_Mesh->SetVertexChannels({ kVertexPosition, kVertexTexcoord0 });
		std::vector<float> vertices = {
			 -1.0f, -1.0f,  -1.0f , 0.0f, 1.0f , // 前
			 -1.0f,  1.0f,  -1.0f , 0.0f, 0.0f ,
			  1.0f,  1.0f,  -1.0f , 1.0f, 0.0f ,
			  1.0f,  -1.0f,  -1.0f , 1.0f, 1.0f ,

			  1.0f, -1.0f,  1.0f , 0.0f, 1.0f , // 后
			 1.0f,  1.0f,  1.0f , 0.0f, 0.0f ,
			  -1.0f,  1.0f,  1.0f , 1.0f, 0.0f ,
			  -1.0f,  -1.0f,  1.0f , 1.0f, 1.0f ,

			  -1.0f, -1.0f,  1.0f , 0.0f, 1.0f , // 左
			 -1.0f,  1.0f,  1.0f , 0.0f, 0.0f ,
			  -1.0f,  1.0f,  -1.0f , 1.0f, 0.0f ,
			  -1.0f,  -1.0f,  -1.0f , 1.0f, 1.0f ,

			  1.0f, -1.0f,  -1.0f , 0.0f, 1.0f , // 右
			 1.0f,  1.0f,  -1.0f , 0.0f, 0.0f ,
			  1.0f,  1.0f,  1.0f , 1.0f, 0.0f ,
			  1.0f,  -1.0f,  1.0f , 1.0f, 1.0f ,

			  -1.0f, 1.0f,  -1.0f , 0.0f, 1.0f , // 上
			 -1.0f,  1.0f,  1.0f , 0.0f, 0.0f ,
			  1.0f,  1.0f,  1.0f , 1.0f, 0.0f ,
			  1.0f,  1.0f,  -1.0f , 1.0f, 1.0f ,

			  -1.0f, -1.0f,  1.0f , 0.0f, 1.0f , // 下
			 -1.0f,  -1.0f,  -1.0f , 0.0f, 0.0f ,
			  1.0f,  -1.0f,  -1.0f , 1.0f, 0.0f ,
			  1.0f,  -1.0f,  1.0f , 1.0f, 1.0f ,
		};
		std::vector<uint32_t> indices = {
			0,1,2, 0,2,3, 4,5,6,  4,6,7, 8,9,10, 8,10,11, 12,13,14, 12,14,15, 16,17,18, 16,18,19, 20,21,22, 20,22,23
		};
		m_Mesh->SetVertices(vertices);
		m_Mesh->SetIndices(indices);
		m_Mesh->UploadToGPU();
	}

	// Texture
	{
		m_Tex = CreateTexture("Tex");
		m_Tex->LoadFromFile(AssetPath + "textures/texture.png");
	}

	// Shader
	{
		m_Shader = CreateShader("Shader");
		m_Shader->LoadSPV(AssetPath + "shaders/E006Instancing/Instancing.vert.spv", AssetPath + "shaders/E006Instancing/Instancing.frag.spv");

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
		parameters.SCParameters.emplace_back(0, kShaderDataInt1);
		m_Shader->CreateGpuProgram(parameters);

		RenderState renderState;

		m_Shader->SetRenderState(renderState);

		m_Shader->SetSCInt(0, 100);
	}

	// Material
	{
		m_Mat = CreateMaterial("Mat");
		m_Mat->SetShader(m_Shader);
		m_Mat->SetTexture("BaseTexture", m_Tex);
	}

	// Instance
	{
		m_InstanceSize = 100 * sizeof(InstanceData);
		m_InstanceData = new InstanceData[100];

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				Transform transform;
				transform.Scale(0.1f, 0.1f, 0.1f).Translate(i * 0.3f, j * 0.3f, 0.0f);

				m_InstanceData[i * 10 + j].model = transform.GetMatrix();
			}
		}
	}
}

MAIN(MakeVulkan)