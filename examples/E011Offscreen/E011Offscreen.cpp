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
#include "RenderPass.h"

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

	// ���InstanceExtension
	dp.enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
	dp.enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	dp.enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

	// ���DeviceExtension
	dp.enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// ���DeviceFeature
#if defined(_WIN32)
	dp.enabledDeviceFeatures.shaderClipDistance = VK_TRUE; // �ҵ��ֻ���֧��
#endif
}

void MakeVulkan::Init()
{
	Example::Init();

	auto& device = GetGfxDevice();
	auto& dp = GetDeviceProperties();

	// Camera
	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 9.0f, -15.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetLens(glm::radians(60.0f), 1.0f * windowWidth / windowHeight, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera->SetSpeed(3.0f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera->SetSpeed(0.005f, 0.005f);
#endif

	PrepareResources();

	RenderPassKey renderPassKeyOffscreen(2, 1, 512, 512);
	renderPassKeyOffscreen.SetAttachment(0, kAttachmentSample, VK_FORMAT_R8G8B8A8_UNORM, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	renderPassKeyOffscreen.SetAttachment(1, kAttachmentDepth, dp.depthFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
	renderPassKeyOffscreen.SetSubpass(0, {}, { 0 }, 1);
	m_RenderPassOffscreen = device.CreateRenderPass(renderPassKeyOffscreen);

	RenderPassKey renderPass(2, 1, windowWidth, windowHeight);
	renderPass.SetAttachment(0, kAttachmentSwapChain, dp.ScFormat.format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	renderPass.SetAttachment(1, kAttachmentDepth, dp.depthFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	renderPass.SetSubpass(0, {}, { 0 }, 1);
	m_RenderPass = device.CreateRenderPass(renderPass);
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

	m_PhongMat->SetFloat2("Clip", -1.0f, m_ClipY);

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
		ImGui::SliderFloat("ClipY", &m_ClipY, -6.0f, 6.0f);
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

	// RenderPassOffscreen

	Attachment* colorAttachmentOffscreen = CreateTempAttachment(kImageColorAspectBit | kImageColorAttachmentBit | kImageSampleBit, VK_FORMAT_R8G8B8A8_UNORM, 512, 512);
	Attachment* depthAttachmentOffscreen = CreateTempAttachment(kImageDepthAspectBit | kImageDepthAttachmentBit, dp.depthFormat, 512, 512);
	m_RenderPassOffscreen->SetAttachments({ colorAttachmentOffscreen, depthAttachmentOffscreen });

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	Rect2D area(0, 0, 512, 512);
	Viewport viewport(0, 0, 512, 512, 0, 1);

	device.BeginRenderPass(m_RenderPassOffscreen, area, clearValues);

	device.SetViewport(viewport);
	device.SetScissor(area);

	SetShader(m_PhongShader);

	BindMaterial(m_PhongMat);
	DrawRenderNode(m_DragonNode);

	device.EndRenderPass();

	// RenderPass

	Attachment* colorAttachment = CreateTempAttachment(kImageSwapChainBit);
	Attachment* depthAttachment = CreateTempAttachment(kImageDepthAspectBit | kImageDepthAttachmentBit, dp.depthFormat, windowWidth, windowHeight);
	m_RenderPass->SetAttachments({ colorAttachment, depthAttachment });

	area.width = windowWidth;
	area.height = windowHeight;
	viewport.width = windowWidth;
	viewport.height = windowHeight;

	device.BeginRenderPass(m_RenderPass, area, clearValues);

	device.SetViewport(viewport);
	device.SetScissor(area);

	SetShader(m_PhongShader);

	BindMaterial(m_PhongMat);
	DrawRenderNode(m_DragonNode);

	SetShader(m_MiniMapShader);

	m_MiniMapMat->SetTexture("BaseTexture", colorAttachmentOffscreen);
	BindMaterial(m_MiniMapMat);
	
	device.BindUniformBuffer(m_MiniMapShader->GetGpuProgram(), 3, 0, nullptr, 0);
	device.BindMeshBuffer(nullptr, nullptr, nullptr);
	device.DrawIndexed(6);

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
	}

	// Shader
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
	{
		m_MiniMapShader = CreateShader("MiniMapShader");
		m_MiniMapShader->LoadSPV(AssetPath + "shaders/E011Offscreen/MiniMap.vert.spv", AssetPath + "shaders/E011Offscreen/MiniMap.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::TextureParameter texture("BaseTexture", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.textureParameters.push_back(texture);
		}
		m_MiniMapShader->CreateGpuProgram(parameters);

		RenderState renderState;
		m_MiniMapShader->SetRenderState(renderState);
	}

	// Material
	{
		m_PhongMat = CreateMaterial("PhongMat");
		m_PhongMat->SetShader(m_PhongShader);
		m_PhongMat->SetFloat2("Clip", -1.0f, m_ClipY);

		m_MiniMapMat = CreateMaterial("MiniMapMat");
		m_MiniMapMat->SetShader(m_MiniMapShader);
	}

	// RenderNode
	{
		m_DragonNode = CreateRenderNode("DragonNode");
		m_DragonNode->SetMesh(m_DragonMesh);
		m_DragonNode->SetMaterial(m_PhongMat);
	}
}

MAIN(MakeVulkan)