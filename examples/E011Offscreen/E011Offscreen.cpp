#include "E011Offscreen.h"
#include "Application.h"
#include "GlobalSettings.h"
#include "GfxDevice.h"
#include "Tools.h"

#include "TimeManager.h"
#include "Imgui.h"
#include "Camera.h"


#include "DeviceProperties.h"//todo
//#include "GfxTypes.h"

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "RenderNode.h"

#include "ResourceManager.h"
#include "RendererScene.h"

#include "GpuProgram.h"
#include "RenderPass.h"

#include "ProfilerManager.h"

MakeVulkan::MakeVulkan()
{
}

MakeVulkan::~MakeVulkan()
{
}

void MakeVulkan::PreInit()
{
	auto& gs = GetGlobalSettings();

#if defined(_WIN32)
	gs.enabledDeviceFeatures.shaderClipDistance = VK_TRUE; // 我的手机不支持
#endif
}

void MakeVulkan::Init()
{
	auto& device = GetGfxDevice();
	VkExtent2D extent = GetGfxDevice().GetSwapChainExtent();

	// Camera
	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 9.0f, -15.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetLens(glm::radians(60.0f), 1.0f * extent.width / extent.height, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera->SetSpeed(3.0f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera->SetSpeed(0.005f, 0.005f);
#endif

	PrepareResources();

	RenderPassKey renderPassKeyOffscreen(2, 1, 512, 512);
	renderPassKeyOffscreen.SetAttachment(0, kAttachmentSample, VK_FORMAT_R8G8B8A8_UNORM, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	renderPassKeyOffscreen.SetAttachment(1, kAttachmentDepth, device.GetDepthFormat(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
	renderPassKeyOffscreen.SetSubpass(0, {}, { 0 }, 1);
	m_RenderPassOffscreen = device.CreateRenderPass(renderPassKeyOffscreen);

	RenderPassKey renderPass(2, 1, extent.width, extent.height);
	renderPass.SetAttachment(0, kAttachmentSwapChain, device.GetSwapChainFormat(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	renderPass.SetAttachment(1, kAttachmentDepth, device.GetDepthFormat(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	renderPass.SetSubpass(0, {}, { 0 }, 1);
	m_RenderPass = device.CreateRenderPass(renderPass);
}

void MakeVulkan::Release()
{
	RELEASE(m_Camera);
}

void MakeVulkan::Update()
{
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

	ImGui::Render();
	m_Imgui->Tick();
}

void MakeVulkan::Draw()
{
	PROFILER(Draw);

	auto& device = GetGfxDevice();
	auto& dp = GetDeviceProperties();

	device.BeginCommandBuffer();

	device.ResetTimeStamp();

	m_RendererScene->BindGlobalUniformBuffer(&m_UniformDataGlobal, sizeof(UniformDataGlobal));
	m_RendererScene->BindPerViewUniformBuffer(&m_UniformDataPerView, sizeof(UniformDataPerView));

	device.WriteTimeStamp("RenderPass");

	// RenderPassOffscreen

	Attachment* colorAttachmentOffscreen = GetResourceManager().CreateTempAttachment(kImageColorAspectBit | kImageColorAttachmentBit | kImageSampleBit, VK_FORMAT_R8G8B8A8_UNORM, 512, 512);
	Attachment* depthAttachmentOffscreen = GetResourceManager().CreateTempAttachment(kImageDepthAspectBit | kImageDepthAttachmentBit, device.GetDepthFormat(), 512, 512);
	m_RenderPassOffscreen->SetAttachments({ colorAttachmentOffscreen, depthAttachmentOffscreen });

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	Rect2D area(0, 0, 512, 512);
	Viewport viewport(0, 0, 512, 512, 0, 1);

	device.BeginRenderPass(m_RenderPassOffscreen, area, clearValues);

	device.SetViewport(viewport);
	device.SetScissor(area);

	m_RendererScene->SetShader(m_PhongShader);

	m_RendererScene->BindMaterial(m_PhongMat);
	m_RendererScene->DrawRenderNode(m_DragonNode);

	device.EndRenderPass();

	// RenderPass

	VkExtent2D extent = GetGfxDevice().GetSwapChainExtent();

	Attachment* colorAttachment = GetResourceManager().CreateTempAttachment(kImageSwapChainBit);
	Attachment* depthAttachment = GetResourceManager().CreateTempAttachment(kImageDepthAspectBit | kImageDepthAttachmentBit, device.GetDepthFormat(), extent.width, extent.height);
	m_RenderPass->SetAttachments({ colorAttachment, depthAttachment });

	area.width = extent.width;
	area.height = extent.height;
	viewport.width = extent.width;
	viewport.height = extent.height;

	device.BeginRenderPass(m_RenderPass, area, clearValues);

	device.SetViewport(viewport);
	device.SetScissor(area);

	m_RendererScene->SetShader(m_PhongShader);

	m_RendererScene->BindMaterial(m_PhongMat);
	m_RendererScene->DrawRenderNode(m_DragonNode);

	m_RendererScene->SetShader(m_MiniMapShader);

	m_MiniMapMat->SetTexture("BaseTexture", colorAttachmentOffscreen);
	m_RendererScene->BindMaterial(m_MiniMapMat);
	
	device.BindUniformBuffer(m_MiniMapShader->GetGpuProgram(), 3, 0, nullptr, 0);
	device.BindMeshBuffer(nullptr, nullptr, nullptr);
	device.DrawIndexed(6);

	// gui

	m_Imgui->Draw();

	device.EndRenderPass();

	device.WriteTimeStamp("RenderPass");

	device.EndCommandBuffer();
}

void MakeVulkan::PrepareResources()
{
	auto& gs = GetGlobalSettings();

	// Mesh
	{
		m_DragonMesh = new Mesh("DragonMesh");
		m_DragonMesh->SetVertexChannels({ kVertexPosition, kVertexNormal });
		m_DragonMesh->LoadFromFile(gs.assetPath + "models/chinesedragon.obj");
		m_DragonMesh->UploadToGPU();
	}

	// Shader
	{
		m_PhongShader = new Shader("PhongShader");
		m_PhongShader->LoadSPV(gs.assetPath + "shaders/E011Offscreen/Phong.vert.spv", gs.assetPath + "shaders/E011Offscreen/Phong.frag.spv");

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
		m_MiniMapShader = new Shader("MiniMapShader");
		m_MiniMapShader->LoadSPV(gs.assetPath + "shaders/E011Offscreen/MiniMap.vert.spv", gs.assetPath + "shaders/E011Offscreen/MiniMap.frag.spv");

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
		m_PhongMat = new Material("PhongMat");
		m_PhongMat->SetShader(m_PhongShader);
		m_PhongMat->SetFloat2("Clip", -1.0f, m_ClipY);

		m_MiniMapMat = new Material("MiniMapMat");
		m_MiniMapMat->SetShader(m_MiniMapShader);
	}

	// RenderNode
	{
		m_DragonNode = new RenderNode("DragonNode");
		m_DragonNode->SetMesh(m_DragonMesh);
		m_DragonNode->SetMaterial(m_PhongMat);
	}
}

//MAIN(MakeVulkan)

#include "Platforms.h"
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)		
{					
	platform::SetWindowInstance(hInstance);

	Application* application = new Application(new MakeVulkan());
	SetApplication(application);
	application->Init();
	application->Run();
	application->Release();
	delete application;

	LOG("Application exits.");
	system("PAUSE");
	return 0;
}