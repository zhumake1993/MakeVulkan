#include "E011Offscreen.h"
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
#include <random>

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

	// Ìí¼ÓInstanceExtension
	dp.enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
	dp.enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	dp.enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

	// Ìí¼ÓDeviceExtension
	dp.enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// Ìí¼ÓDeviceFeature
	dp.enabledDeviceFeatures.shaderClipDistance = VK_TRUE;
}

void MakeVulkan::Init()
{
	Example::Init();

	auto& device = GetGfxDevice();
	auto& dp = GetDeviceProperties();

	// Camera
	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 12.0f, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetLens(glm::radians(60.0f), 1.0f * windowWidth / windowHeight, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera->SetSpeed(3.0f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera->SetSpeed(0.005f, 0.005f);
#endif

	PrepareResources();

	// RenderPass
	{
		// Attachments
		m_RenderPassOffscreen.attachmentDescs.resize(2);
		std::vector<AttachmentDesc>& attachmentDescs = m_RenderPassOffscreen.attachmentDescs;

		attachmentDescs[0].format = dp.ScFormat.format;
		attachmentDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		attachmentDescs[1].format = dp.depthFormat;
		attachmentDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		// Subpasses
		m_RenderPassOffscreen.subPassDescs.resize(1);
		std::vector<SubPassDesc>& subPassDescs = m_RenderPassOffscreen.subPassDescs;

		subPassDescs[0].colors = { 0 };
		subPassDescs[0].useDepthStencil = true;

		m_RenderPassOffscreen.present = 0;
		m_RenderPassOffscreen.depthStencil = 1;
	}
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

	m_UniformDataPerView.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);
	m_UniformDataPerView.strength = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	m_UniformDataPerView.direction = glm::vec4(1.0f, -1.0f, 0.0f, 0.0f);

	m_MirrorPhongMat->SetFloat2("Clip", 1.0f, m_ClipY);
	m_PhongMat->SetFloat2("Clip", -1.0f, m_ClipY);

	m_MirrorDragonNode->GetTransform().Rotate(deltaTime * 0.25f, 0.0f, 1.0f, 0.0f);
	m_DragonNode->GetTransform().Rotate(deltaTime * 0.25f, 0.0f, 1.0f, 0.0f);

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

	if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_None))
	{
		ImGui::SliderFloat("ClipY", &m_ClipY, -5.0f, 5.0f);
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

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	Rect2D area(0, 0, windowWidth, windowHeight);
	Viewport viewport(0, 0, windowWidth, windowHeight, 0, 1);

	device.WriteTimeStamp("RenderPass");

	device.SetRenderPass(m_RenderPassOffscreen);

	device.BeginRenderPass(area, clearValues);

	device.SetViewport(viewport);
	device.SetScissor(area);

	// First

	SetShader(m_MirrorPhongShader);

	BindMaterial(m_MirrorPhongMat);
	DrawRenderNode(m_MirrorDragonNode);

	//

	SetShader(m_PhongShader);

	BindMaterial(m_PhongMat);
	DrawRenderNode(m_DragonNode);

	// gui

	DrawImgui();

	device.EndRenderPass();

	device.WriteTimeStamp("RenderPass");

	device.EndCommandBuffer();
}

void MakeVulkan::PrepareResources()
{
	// Mesh
	{
		m_DragonMesh = CreateMesh("DragonMesh");
		m_DragonMesh->SetVertexChannels({ kVertexPosition, kVertexNormal });
		m_DragonMesh->LoadFromFile(AssetPath + "models/chinesedragon.obj");
		m_DragonMesh->UploadToGPU();

		m_PlaneMesh = CreateMesh("PlaneMesh");
		m_PlaneMesh->SetVertexChannels({ kVertexPosition, kVertexNormal });
		m_PlaneMesh->LoadFromFile(AssetPath + "models/plane.obj");
		m_PlaneMesh->UploadToGPU();
	}

	// Shader
	{
		m_MirrorPhongShader = CreateShader("MirrorPhongShader");
		m_MirrorPhongShader->LoadSPV(AssetPath + "shaders/E011Offscreen/Phong.vert.spv", AssetPath + "shaders/E011Offscreen/Phong.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", kShaderDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		{
			GpuParameters::UniformParameter uniform("PerMaterial", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("Clip", kShaderDataFloat2);
			parameters.uniformParameters.push_back(uniform);
		}
		m_MirrorPhongShader->CreateGpuProgram(parameters);

		RenderState renderState;
		renderState.rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
		m_MirrorPhongShader->SetRenderState(renderState);
	}
	{
		m_PhongShader = CreateShader("PhongShader");
		m_PhongShader->LoadSPV(AssetPath + "shaders/E011Offscreen/Phong.vert.spv", AssetPath + "shaders/E011Offscreen/Phong.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", kShaderDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		{
			GpuParameters::UniformParameter uniform("PerMaterial", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("Clip", kShaderDataFloat2);
			parameters.uniformParameters.push_back(uniform);
		}
		m_PhongShader->CreateGpuProgram(parameters);

		RenderState renderState;
		m_PhongShader->SetRenderState(renderState);
	}

	// Material
	{
		m_MirrorPhongMat = CreateMaterial("MirrorPhongMat");
		m_MirrorPhongMat->SetShader(m_MirrorPhongShader);
		m_MirrorPhongMat->SetFloat2("Clip", 1.0f, m_ClipY);

		m_PhongMat = CreateMaterial("PhongMat");
		m_PhongMat->SetShader(m_PhongShader);
		m_PhongMat->SetFloat2("Clip", -1.0f, m_ClipY);
	}

	// RenderNode
	{
		m_MirrorDragonNode = CreateRenderNode("MirrorDragonNode");
		m_MirrorDragonNode->SetMesh(m_DragonMesh);
		m_MirrorDragonNode->SetMaterial(m_MirrorPhongMat);
		m_MirrorDragonNode->GetTransform().Scale(1.0f, -1.0f, 1.0f).Translate(0.0f, -6.0f, 0.0f);

		m_DragonNode = CreateRenderNode("DragonNode");
		m_DragonNode->SetMesh(m_DragonMesh);
		m_DragonNode->SetMaterial(m_PhongMat);
		m_DragonNode->GetTransform().Scale(1.0f, 1.0f, 1.0f).Translate(0.0f, 6.0f, 0.0f);

		//m_PlaneNode = CreateRenderNode("PlaneNode");
		//m_PlaneNode->SetMesh(m_GlassMesh);
		//m_PlaneNode->SetMaterial(m_TransparentMat);
	}
}

MAIN(MakeVulkan)