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
#include "RenderPass.h"
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
	dp.enabledDeviceFeatures.samplerAnisotropy = VK_TRUE;
}

void MakeVulkan::Init()
{
	Example::Init();

	auto& device = GetGfxDevice();
	auto& dp = GetDeviceProperties();

	// Camera
	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 5.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetLens(glm::radians(60.0f), 1.0f * windowWidth / windowHeight, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera->SetSpeed(3.0f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera->SetSpeed(0.005f, 0.005f);
#endif

	PrepareResources();

	RenderPassKey renderPassKey(5, 3, windowWidth, windowHeight);
	renderPassKey.SetAttachment(0, kAttachmentSwapChain, dp.ScFormat.format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	renderPassKey.SetAttachment(1, kAttachmentColor, VK_FORMAT_R16G16B16A16_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
	renderPassKey.SetAttachment(2, kAttachmentColor, VK_FORMAT_R16G16B16A16_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
	renderPassKey.SetAttachment(3, kAttachmentColor, VK_FORMAT_R8G8B8A8_UNORM, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
	renderPassKey.SetAttachment(4, kAttachmentDepth, dp.depthFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
	renderPassKey.SetSubpass(0, {}, { 0,1,2,3 }, 4);
	renderPassKey.SetSubpass(1, { 1,2,3 }, { 0 }, 4);
	renderPassKey.SetSubpass(2, { 1 }, { 0 }, 4);
	m_RenderPass = device.CreateRenderPass(renderPassKey);

	{
		std::vector<glm::vec3> colors =
		{
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		};

		std::default_random_engine rndGen((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(-1.0f, 1.0f);
		std::uniform_int_distribution<uint32_t> rndCol(0, static_cast<uint32_t>(colors.size() - 1));

		for (auto& light : m_UniformDataPerView.lights)
		{
			light.position = glm::vec4(rndDist(rndGen) * 10.0f, 0.25f + std::abs(rndDist(rndGen)) * 8.0f, rndDist(rndGen) * 10.0f, 1.0f);
			light.color = colors[rndCol(rndGen)];
			light.radius = 1.0f + std::abs(rndDist(rndGen));
		}
	}
}

void MakeVulkan::Release()
{
	RELEASE(m_RenderPass);
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

	for (auto& light : m_UniformDataPerView.lights)
	{
		light.position = glm::rotate(glm::mat4(1.0f), deltaTime * 0.5f, glm::vec3(0, 1.0f, 0)) * light.position;
	}

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
	auto& dp = GetDeviceProperties();

	device.BeginCommandBuffer();

	device.ResetTimeStamp();

	BindGlobalUniformBuffer(&m_UniformDataGlobal, sizeof(UniformDataGlobal));
	BindPerViewUniformBuffer(&m_UniformDataPerView, sizeof(UniformDataPerView));

	device.WriteTimeStamp("RenderPass");

	// RenderPass
	Attachment* colorAttachment = CreateTempAttachment(kImageSwapChainBit);
	Attachment* positionAttachment = CreateTempAttachment(kImageColorAspectBit | kImageColorAttachmentBit | kImageInputAttachmentBit, VK_FORMAT_R16G16B16A16_SFLOAT, windowWidth, windowHeight);
	Attachment* normalAttachment = CreateTempAttachment(kImageColorAspectBit | kImageColorAttachmentBit | kImageInputAttachmentBit, VK_FORMAT_R16G16B16A16_SFLOAT, windowWidth, windowHeight);
	Attachment* albedoAttachment = CreateTempAttachment(kImageColorAspectBit | kImageColorAttachmentBit | kImageInputAttachmentBit, VK_FORMAT_R8G8B8A8_UNORM, windowWidth, windowHeight);
	Attachment* depthAttachment = CreateTempAttachment(kImageDepthAspectBit | kImageDepthAttachmentBit, dp.depthFormat, windowWidth, windowHeight);
	m_RenderPass->SetAttachments({ colorAttachment, positionAttachment, normalAttachment, albedoAttachment, depthAttachment });

	std::vector<VkClearValue> clearValues(5);
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[1].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[2].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[3].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[4].depthStencil = { 1.0f, 0 };

	Rect2D area(0, 0, windowWidth, windowHeight);
	Viewport viewport(0, 0, windowWidth, windowHeight, 0, 1);

	device.BeginRenderPass(m_RenderPass, area, clearValues);

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

	// Third subpass
	// Render transparent geometry using a forward pass that compares against depth generted during G-Buffer fill

	device.NextSubpass();

	SetShader(m_TransparentShader);

	BindMaterial(m_TransparentMat);
	DrawRenderNode(m_GlassNode);

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
		m_BuildingMesh->SetVertexChannels({ kVertexPosition, kVertexNormal, kVertexColor });
		m_BuildingMesh->LoadFromFile(AssetPath + "models/samplebuilding.obj");
		m_BuildingMesh->UploadToGPU();

		m_GlassMesh = CreateMesh("GlassMesh");
		m_GlassMesh->SetVertexChannels({ kVertexPosition, kVertexNormal, kVertexColor, kVertexTexcoord0 });
		m_GlassMesh->LoadFromFile(AssetPath + "models/samplebuilding_glass.obj");
		m_GlassMesh->UploadToGPU();
	}

	// Texture
	{
		m_GlassTex = CreateTexture("GlassTex");
		m_GlassTex->LoadFromFile(AssetPath + "textures/colored_glass_rgba.ktx");
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
		parameters.SCParameters.emplace_back(0, kShaderDataInt1);
		m_CompositionShader->CreateGpuProgram(parameters);

		RenderState renderState;
		renderState.depthStencilState.depthWriteEnable = VK_FALSE;
		m_CompositionShader->SetRenderState(renderState);

		m_CompositionShader->SetSCInt(0, m_LightCount);
	}
	{
		m_TransparentShader = CreateShader("TransparentShader");
		m_TransparentShader->LoadSPV(AssetPath + "shaders/E010SubPasses/Transparent.vert.spv", AssetPath + "shaders/E010SubPasses/Transparent.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", kShaderDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		{
			GpuParameters::InputAttachmentParameter texture("PositionTexture", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.inputAttachmentParameters.push_back(texture);
		}
		{
			GpuParameters::TextureParameter texture("BaseTexture", 1, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.textureParameters.push_back(texture);
		}
		m_TransparentShader->CreateGpuProgram(parameters);

		RenderState renderState;
		renderState.depthStencilState.depthWriteEnable = VK_FALSE;
		renderState.rasterizationState.cullMode = VK_CULL_MODE_NONE;
		renderState.blendStates[0].blendEnable = VK_TRUE;
		renderState.blendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		renderState.blendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		renderState.blendStates[0].colorBlendOp = VK_BLEND_OP_ADD;
		m_TransparentShader->SetRenderState(renderState);
	}

	// Material
	{
		m_GBufferMat = CreateMaterial("GBufferMat");
		m_GBufferMat->SetShader(m_GBufferShader);

		m_CompositionMat = CreateMaterial("CompositionMat");
		m_CompositionMat->SetShader(m_CompositionShader);

		m_TransparentMat = CreateMaterial("TransparentMat");
		m_TransparentMat->SetShader(m_TransparentShader);
		m_TransparentMat->SetTexture("BaseTexture", m_GlassTex);
	}

	// RenderNode
	{
		m_BuildingNode = CreateRenderNode("BuildingNode");
		m_BuildingNode->SetMesh(m_BuildingMesh);
		m_BuildingNode->SetMaterial(m_GBufferMat);

		m_GlassNode = CreateRenderNode("GlassNode");
		m_GlassNode->SetMesh(m_GlassMesh);
		m_GlassNode->SetMaterial(m_TransparentMat);
	}
}

MAIN(MakeVulkan)