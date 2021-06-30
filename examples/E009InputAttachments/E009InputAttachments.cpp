#include "E009InputAttachments.h"
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

	//// RenderPass
	//{
	//	// Attachments
	//	m_RenderPassDesc.attachmentDescs.resize(2);
	//	mkVector<AttachmentDesc>& attachmentDescs = m_RenderPassDesc.attachmentDescs;

	//	attachmentDescs[0].format = dp.ScFormat.format;
	//	attachmentDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//	attachmentDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	//	attachmentDescs[1].format = dp.depthFormat;
	//	attachmentDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//	attachmentDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	//	// Subpasses
	//	m_RenderPassDesc.subPassDescs.resize(1);
	//	mkVector<SubPassDesc>& subPassDescs = m_RenderPassDesc.subPassDescs;

	//	subPassDescs[0].colors = { 0 };
	//	subPassDescs[0].useDepthStencil = true;

	//	m_RenderPassDesc.present = 0;
	//	m_RenderPassDesc.depthStencil = 1;
	//}

	// RenderPass
	{
		// Attachments
		m_RenderPassDesc.attachmentDescs.resize(3);
		mkVector<AttachmentDesc>& attachmentDescs = m_RenderPassDesc.attachmentDescs;

		attachmentDescs[0].format = dp.ScFormat.format;
		attachmentDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		attachmentDescs[1].format = VK_FORMAT_R8G8B8A8_UNORM;
		attachmentDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		attachmentDescs[2].format = dp.depthFormat;
		attachmentDescs[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[2].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// Subpasses
		m_RenderPassDesc.subPassDescs.resize(2);
		mkVector<SubPassDesc>& subPassDescs = m_RenderPassDesc.subPassDescs;

		subPassDescs[0].colors = { 1 };
		subPassDescs[0].useDepthStencil = true;

		subPassDescs[1].inputs = { 1,2 };
		subPassDescs[1].colors = { 0 };
		subPassDescs[1].useDepthStencil = false;

		m_RenderPassDesc.present = 0;
		m_RenderPassDesc.depthStencil = 2;
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

	m_Node->GetTransform().Rotate(-deltaTime * 0.5f, 0.0f, 1.0f, 0.0f);

	m_FetchMat->SetFloat4("Para", m_Brightness, m_Contrast, m_DepthMin, m_DepthMax);
	m_FetchMat->SetInt("Depth", m_FetchDepth ? 1 : 0);

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

	if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_None))
	{
		ImGui::Checkbox("Fetch Depth", &m_FetchDepth);
		ImGui::SliderFloat("Brightness", &m_Brightness, 0.0f, 2.0f);
		ImGui::SliderFloat("Contrast", &m_Contrast, 0.0f, 4.0f);
		ImGui::SliderFloat("DepthMin", &m_DepthMin, 0.0f, 1.0f);
		ImGui::SliderFloat("DepthMax", &m_DepthMax, 0.0f, 1.0f);

		if (m_DepthMin > m_DepthMax)
		{
			m_DepthMin = m_DepthMax;
		}
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

	mkVector<VkClearValue> clearValues(3);
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[1].color = { 0.0f, 0.0f, 0.2f, 0.0f };
	clearValues[2].depthStencil = { 1.0f, 0 };

	Rect2D area(0, 0, windowWidth, windowHeight);
	Viewport viewport(0, 0, windowWidth, windowHeight, 0, 1);

	device.WriteTimeStamp("RenderPass");

	device.SetRenderPass(m_RenderPassDesc);

	device.BeginRenderPass(area, clearValues);

	device.SetViewport(viewport);
	device.SetScissor(area);

	// First sub pass

	SetShader(m_Shader);

	BindMaterial(m_Mat);

	DrawRenderNode(m_Node);

	// Second sub pass

	device.NextSubpass();

	SetShader(m_FetchShader);

	BindMaterial(m_FetchMat);

	device.BindUniformBuffer(m_FetchShader->GetGpuProgram(), 3, 0, nullptr, 0);
	device.BindMeshBuffer(nullptr, nullptr, nullptr);
	device.DrawIndexed(3); // gl_VertexIndex的值可能是0 2 4，似乎跟VkIndexType是16位还是32位有关。把DrawImgui放前面，就是0 2 4

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
		m_Mesh->SetVertexChannels({ kVertexPosition, kVertexColor });
		mkVector<float> vertices = {
			-1.0f, -1.0f,  1.0f , 1.0f, 0.0f, 0.0f  ,
			1.0f, -1.0f,  1.0f , 0.0f, 1.0f, 0.0f  ,
			1.0f,  1.0f,  1.0f , 0.0f, 0.0f, 1.0f  ,
			-1.0f,  1.0f,  1.0f , 0.0f, 0.0f, 0.0f  ,
			-1.0f, -1.0f, -1.0f , 1.0f, 0.0f, 0.0f  ,
			1.0f, -1.0f, -1.0f , 0.0f, 1.0f, 0.0f  ,
			1.0f,  1.0f, -1.0f , 0.0f, 0.0f, 1.0f  ,
			-1.0f,  1.0f, -1.0f , 0.0f, 0.0f, 0.0f  ,
		};
		mkVector<uint32_t> indices = {
			0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 4,5,1, 1,0,4, 3,2,6, 6,7,3,
		};
		m_Mesh->SetVertices(vertices);
		m_Mesh->SetIndices(indices);
		m_Mesh->UploadToGPU();
	}

	// Texture
	{
	}

	// Shader
	{
		m_Shader = CreateShader("Shader");
		m_Shader->LoadSPV(AssetPath + "shaders/E009InputAttachments/Color.vert.spv", AssetPath + "shaders/E009InputAttachments/Color.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", kShaderDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		m_Shader->CreateGpuProgram(parameters);

		RenderState renderState;

		m_Shader->SetRenderState(renderState);
	}
	{
		m_FetchShader = CreateShader("FetchShader");
		m_FetchShader->LoadSPV(AssetPath + "shaders/E009InputAttachments/Fetch.vert.spv", AssetPath + "shaders/E009InputAttachments/Fetch.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::InputAttachmentParameter texture("ColorTexture", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.inputAttachmentParameters.push_back(texture);
		}
		{
			GpuParameters::InputAttachmentParameter texture("DepthTexture", 1, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.inputAttachmentParameters.push_back(texture);
		}
		{
			GpuParameters::UniformParameter uniform("PerMaterial", 2, VK_SHADER_STAGE_FRAGMENT_BIT);
			uniform.valueParameters.emplace_back("Para", kShaderDataFloat4);
			uniform.valueParameters.emplace_back("Depth", kShaderDataInt1);
			parameters.uniformParameters.push_back(uniform);
		}
		m_FetchShader->CreateGpuProgram(parameters);

		RenderState renderState;
		m_FetchShader->SetRenderState(renderState);
	}

	// Material
	{
		m_Mat = CreateMaterial("Mat");
		m_Mat->SetShader(m_Shader);

		m_FetchMat = CreateMaterial("FetchMat");
		m_FetchMat->SetShader(m_FetchShader);
		m_FetchMat->SetFloat4("Para", m_Brightness, m_Contrast, m_DepthMin, m_DepthMax);
		m_FetchMat->SetInt("Depth", m_FetchDepth ? 1 : 0);
	}

	// RenderNode
	{
		m_Node = CreateRenderNode("Node");
		m_Node->SetMesh(m_Mesh);
		m_Node->SetMaterial(m_Mat);
	}
}

MAIN(MakeVulkan)