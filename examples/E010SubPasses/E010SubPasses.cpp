#include "E010SubPasses.h"
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

	// ����InstanceExtension
	dp.enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
	dp.enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	dp.enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

	// ����DeviceExtension
	dp.enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// ����DeviceFeature
	dp.enabledDeviceFeatures.samplerAnisotropy = VK_TRUE;
}

void MakeVulkan::Init()
{
	Example::Init();

	auto& device = GetGfxDevice();
	auto& dp = GetDeviceProperties();

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

	// RenderPass
	{
		// Attachments
		m_RenderPassDesc.attachmentDescs.resize(5);
		std::vector<AttachmentDesc>& attachmentDescs = m_RenderPassDesc.attachmentDescs;

		// Color attachment
		attachmentDescs[0].format = dp.ScFormat.format;
		attachmentDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		// Deferred attachments

		// Position
		attachmentDescs[1].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		attachmentDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// Normals
		attachmentDescs[2].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		attachmentDescs[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[2].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// Albedo
		attachmentDescs[3].format = VK_FORMAT_R8G8B8A8_UNORM;
		attachmentDescs[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[3].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// Depth attachment
		attachmentDescs[4].format = dp.depthFormat;
		attachmentDescs[4].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[4].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// Subpasses
		m_RenderPassDesc.subPassDescs.resize(2);
		std::vector<SubPassDesc>& subPassDescs = m_RenderPassDesc.subPassDescs;

		subPassDescs[0].colors = { 0,1,2,3 };
		subPassDescs[0].useDepthStencil = true;

		subPassDescs[1].inputs = { 1,2,3 };
		subPassDescs[1].colors = { 0 };
		subPassDescs[1].useDepthStencil = true;

		/*subPassDescs[2].inputs = { 1 };
		subPassDescs[2].colors = { 0 };
		subPassDescs[2].useDepthStencil = true;*/

		m_RenderPassDesc.present = 0;
		m_RenderPassDesc.depthStencil = 4;
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

	m_CompositionMat->SetInt("Mode", m_Mode);

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
		std::vector<const char*> items = { "Position","Color","Normal" };
		uint32_t itemCount = static_cast<uint32_t>(items.size());
		bool res = ImGui::Combo("Mode", &m_Mode, items.data(), itemCount, itemCount);
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

	std::vector<VkClearValue> clearValues(5);
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[1].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[2].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[3].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[4].depthStencil = { 1.0f, 0 };

	Rect2D area(0, 0, windowWidth, windowHeight);
	Viewport viewport(0, 0, windowWidth, windowHeight, 0, 1);

	device.WriteTimeStamp("RenderPass");

	device.SetRenderPass(m_RenderPassDesc);

	device.BeginRenderPass(area, clearValues);

	device.SetViewport(viewport);
	device.SetScissor(area);

	// First sub pass
	// Renders the components of the scene to the G-Buffer atttachments

	SetShader(m_GBufferShader);

	BindMaterial(m_GBufferMat);
	DrawRenderNode(m_BuildingNode);

	// Second sub pass
	// This subpass will use the G-Buffer components that have been filled in the first subpass as input attachment for the final compositing

	device.NextSubpass();

	SetShader(m_CompositionShader);

	BindMaterial(m_CompositionMat);

	device.BindUniformBuffer(m_CompositionShader->GetGpuProgram(), 3, 0, nullptr, 0);//??
	device.BindMeshBuffer(nullptr, nullptr, nullptr);
	device.DrawIndexed(3);

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
		m_BuildingMesh = CreateMesh("BuildingMesh");
		m_BuildingMesh->SetVertexChannels({ kVertexPosition, kVertexColor, kVertexNormal });
		m_BuildingMesh->LoadFromFile(AssetPath + "models/samplebuilding.obj");
		m_BuildingMesh->UploadToGPU();

		m_GlassMesh = CreateMesh("GlassMesh");
		m_GlassMesh->SetVertexChannels({ kVertexPosition, kVertexColor, kVertexNormal });
		m_GlassMesh->LoadFromFile(AssetPath + "models/samplebuilding_glass.obj");
		m_GlassMesh->UploadToGPU();
	}

	// Texture
	{
		m_Tex = CreateTexture("Tex");
		m_Tex->LoadFromFile(AssetPath + "textures/colored_glass_rgba.ktx");
	}

	// Shader
	{
		m_GBufferShader = CreateShader("GBufferShader");
		m_GBufferShader->LoadSPV(AssetPath + "shaders/E010SubPasses/GBuffer.vert.spv", AssetPath + "shaders/E010SubPasses/GBuffer.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", kShaderDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		m_GBufferShader->CreateGpuProgram(parameters);

		RenderState renderState;
		renderState.blendStates.resize(4);
		m_GBufferShader->SetRenderState(renderState);
	}
	{
		m_CompositionShader = CreateShader("CompositionShader");
		m_CompositionShader->LoadSPV(AssetPath + "shaders/E010SubPasses/Composition.vert.spv", AssetPath + "shaders/E010SubPasses/Composition.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::InputAttachmentParameter texture("PositionTexture", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.inputAttachmentParameters.push_back(texture);
		}
		{
			GpuParameters::InputAttachmentParameter texture("ColorTexture", 1, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.inputAttachmentParameters.push_back(texture);
		}
		{
			GpuParameters::InputAttachmentParameter texture("NormalTexture", 2, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.inputAttachmentParameters.push_back(texture);
		}
		{
			GpuParameters::UniformParameter uniform("PerMaterial", 3, VK_SHADER_STAGE_FRAGMENT_BIT);
			uniform.valueParameters.emplace_back("Mode", kShaderDataInt1);
			parameters.uniformParameters.push_back(uniform);
		}
		m_CompositionShader->CreateGpuProgram(parameters);

		RenderState renderState;
		m_CompositionShader->SetRenderState(renderState);
	}

	// Material
	{
		m_GBufferMat = CreateMaterial("GBufferMat");
		m_GBufferMat->SetShader(m_GBufferShader);

		m_CompositionMat = CreateMaterial("CompositionMat");
		m_CompositionMat->SetShader(m_CompositionShader);
	}

	// RenderNode
	{
		m_BuildingNode = CreateRenderNode("BuildingNode");
		m_BuildingNode->SetMesh(m_BuildingMesh);
		m_BuildingNode->SetMaterial(m_GBufferMat);
	}
}

MAIN(MakeVulkan)