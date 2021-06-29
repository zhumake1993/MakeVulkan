#include "E008Cubemap.h"
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
	if (dp.deviceFeatures.imageCubeArray)
		dp.enabledDeviceFeatures.imageCubeArray = VK_TRUE;
	else
		LOGE("Selected GPU does not support cube map arrays!");

	if (dp.deviceFeatures.samplerAnisotropy)
		dp.enabledDeviceFeatures.samplerAnisotropy = VK_TRUE;
	else
		LOGE("Selected GPU does not support samplerAnisotropy!");
}

void MakeVulkan::Init()
{
	Example::Init();

	auto& device = GetGfxDevice();

	// Camera
	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 4.0f, -8.0f), glm::vec3(0.0f, 0.0f, 0.0f));
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
	m_UniformDataPerView.invView = glm::inverse(m_UniformDataPerView.view);

	m_ReflectMat->SetInt("TextureIndex", m_TextureIndex);
	m_ReflectMat->SetFloat("LodBias", m_LodBias);

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
		ImGui::SliderInt("Mesh", &m_MeshIndex, 0, 2);
		ImGui::SliderInt("Texture", &m_TextureIndex, 0, static_cast<int>(m_Tex->GetLayerCount()) - 1);
		ImGui::SliderFloat("lofBias", &m_LodBias, 0, static_cast<float>(m_Tex->GetMipCount() - 1));
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

	SetShader(m_ReflectShader);

	BindMaterial(m_ReflectMat);

	DrawRenderNode(m_Nodes[m_MeshIndex]);

	SetShader(m_SkyboxShader);

	BindMaterial(m_SkyboxMat);

	DrawRenderNode(m_Nodes[3]);

	DrawImgui();

	device.EndRenderPass();

	device.WriteTimeStamp("RenderPass");

	device.EndCommandBuffer();
}

void MakeVulkan::PrepareResources()
{
	// Mesh
	{
		m_SphereMesh = CreateMesh("SphereMesh");
		m_SphereMesh->SetVertexChannels({ kVertexPosition, kVertexNormal });
		m_SphereMesh->LoadFromFile(AssetPath + "models/sphere.obj");
		m_SphereMesh->UploadToGPU();

		m_TorusMesh = CreateMesh("TorusMesh");
		m_TorusMesh->SetVertexChannels({ kVertexPosition, kVertexNormal });
		m_TorusMesh->LoadFromFile(AssetPath + "models/torus.obj");
		m_TorusMesh->UploadToGPU();

		m_TorusknotMesh = CreateMesh("TorusknotMesh");
		m_TorusknotMesh->SetVertexChannels({ kVertexPosition, kVertexNormal });
		m_TorusknotMesh->LoadFromFile(AssetPath + "models/torusknot.obj");
		m_TorusknotMesh->UploadToGPU();

		m_SkyboxMesh = CreateMesh("SkyboxMesh");
		m_SkyboxMesh->SetVertexChannels({ kVertexPosition });
		m_SkyboxMesh->LoadFromFile(AssetPath + "models/cube.obj");
		m_SkyboxMesh->UploadToGPU();
	}

	// Texture
	{
		m_Tex = CreateTexture("Tex");
		m_Tex->LoadFromFile(AssetPath + "textures/cubemap_array.ktx", VK_FORMAT_R8G8B8A8_UNORM, true);
	}

	// Shader
	{
		m_ReflectShader = CreateShader("ReflectShader");
		m_ReflectShader->LoadSPV(AssetPath + "shaders/E008Cubemap/Reflect.vert.spv", AssetPath + "shaders/E008Cubemap/Reflect.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerMaterial", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			uniform.valueParameters.emplace_back("TextureIndex", kShaderDataInt1);
			uniform.valueParameters.emplace_back("LodBias", kShaderDataFloat1);
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
		m_ReflectShader->CreateGpuProgram(parameters);

		RenderState renderState;
		m_ReflectShader->SetRenderState(renderState);
	}
	{
		m_SkyboxShader = CreateShader("SkyboxShader");
		m_SkyboxShader->LoadSPV(AssetPath + "shaders/E008Cubemap/Skybox.vert.spv", AssetPath + "shaders/E008Cubemap/Skybox.frag.spv");

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
		m_SkyboxShader->CreateGpuProgram(parameters);

		RenderState renderState;
		renderState.rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
		m_SkyboxShader->SetRenderState(renderState);
	}

	// Material
	{
		m_ReflectMat = CreateMaterial("ReflectMat");
		m_ReflectMat->SetShader(m_ReflectShader);
		m_ReflectMat->SetTexture("BaseTexture", m_Tex);

		m_SkyboxMat = CreateMaterial("SkyboxMat");
		m_SkyboxMat->SetShader(m_SkyboxShader);
		m_SkyboxMat->SetTexture("BaseTexture", m_Tex);
	}

	// RenderNode
	{
		RenderNode* sphereNode = CreateRenderNode("SphereNode");
		sphereNode->SetMesh(m_SphereMesh);
		sphereNode->SetMaterial(m_ReflectMat);
		sphereNode->GetTransform().Scale(0.1f, 0.1f, 0.1f);

		RenderNode* torusNode = CreateRenderNode("TorusNode");
		torusNode->SetMesh(m_TorusMesh);
		torusNode->SetMaterial(m_ReflectMat);
		torusNode->GetTransform().Scale(0.05f, 0.05f, 0.05f);

		RenderNode* torusknotNode = CreateRenderNode("TorusknotNode");
		torusknotNode->SetMesh(m_TorusknotMesh);
		torusknotNode->SetMaterial(m_ReflectMat);
		torusknotNode->GetTransform().Scale(0.1f, 0.1f, 0.1f);

		RenderNode* skyboxNode = CreateRenderNode("SkyboxNode");
		skyboxNode->SetMesh(m_SkyboxMesh);
		skyboxNode->SetMaterial(m_SkyboxMat);
		skyboxNode->GetTransform().Scale(10.0f, 10.0f, 10.0f);

		m_Nodes.push_back(sphereNode);
		m_Nodes.push_back(torusNode);
		m_Nodes.push_back(torusknotNode);
		m_Nodes.push_back(skyboxNode);
	}
}

MAIN(MakeVulkan)