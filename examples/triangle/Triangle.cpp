#include "Triangle.h"
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

#include "GpuProgram.h"

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void Triangle::ConfigDeviceProperties()
{
	auto& dp = GetDeviceProperties();

	// 添加InstanceLayer

	// vkAllocateDescriptorSets发生内存泄露的原因跟VK_LAYER_KHRONOS_validation有关
	// 可能的原因请参考：https ://github.com/KhronosGroup/Vulkan-ValidationLayers/issues/236
	// 使用VK_PRESENT_MODE_MAILBOX_KHR时，不管是vkFreeDescriptorSets还是vkResetDescriptorPool都无法解决内存泄漏
	// 使用VK_PRESENT_MODE_FIFO_KHR时，没有观察到内存泄露
	// 升级Vulkan至1.2.162.1，问题仍在

	// 安卓上VK_LAYER_KHRONOS_validation需要至少NDK r20
	// 目前我用的NDK r19
#if defined(_WIN32)
	dp.enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// 添加InstanceExtension

	// 这个暂时搁置
	//dp.enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	dp.enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
	dp.enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	dp.enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

	// 添加DeviceExtension

	dp.enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// 添加DeviceFeature

	//dp.enabledDeviceFeatures.fillModeNonSolid = VK_TRUE;
	//dp.enabledDeviceFeatures.wideLines = VK_TRUE;
	//dp.enabledDeviceFeatures.samplerAnisotropy = VK_TRUE;
}

void Triangle::Init()
{
	Example::Init();

	auto& device = GetGfxDevice();

	// Camera
	m_Camera = new Camera();
	m_Camera->LookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetLens(glm::radians(60.0f), 1.0f * windowWidth / windowHeight, 0.1f, 256.0f);
#if defined(_WIN32)
	m_Camera->SetSpeed(0.5f, 0.005f);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_Camera->SetSpeed(0.001f, 0.005f);
#endif

	PrepareResources();
}

void Triangle::Release()
{
	RELEASE(m_Camera);

	Example::Release();
}

void Triangle::Update()
{
	Example::Update();

	float deltaTime = m_TimeManager->GetDeltaTime();

	m_UniformDataGlobal.time = glm::vec4(0, 0, 0, 0);

	m_Camera->Update(deltaTime);

	m_UniformDataPerView.view = m_Camera->GetView();
	m_UniformDataPerView.proj = m_Camera->GetProj();
	m_UniformDataPerView.eyePos = glm::vec4(m_Camera->GetPosition(), 1.0f);

	m_UniformDataPerView.ambientLight = glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);

	m_UniformDataPerView.lights[0].strength = glm::vec3(0.3f, 0.3f, 0.3f);
	m_UniformDataPerView.lights[0].direction = glm::vec3(1.0f, 0.0f, 0.0f);

	/*m_UniformDataPerView.lights[1].strength = glm::vec3(0.3f, 0.3f, 0.3f);
	m_UniformDataPerView.lights[1].falloffStart = 0.1f;
	m_UniformDataPerView.lights[1].falloffEnd = 1.0f;
	m_UniformDataPerView.lights[1].position = glm::vec3(0.0f, 0.0f, -1.0f);*/

	m_ColorCubeNode->GetTransform().Rotate(-deltaTime * 0.5f, 0.0f, 1.0f, 0.0f);
	m_TexCubeNode->GetTransform().Rotate(deltaTime * 0.5f, 0.0f, 1.0f, 0.0f);
	m_LitSphereNode->GetTransform().Rotate(-deltaTime * 0.5f, 0.0f, 1.0f, 0.0f);

	// Imgui

	// UI样例，供学习用
	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);

	UpdateImgui();
}

void Triangle::Draw()
{
	auto& device = GetGfxDevice();

	device.BeginCommandBuffer();

	BindGlobalUniformBuffer();
	BindPerViewUniformBuffer();

	Color clearColor(0, 0, 0, 0);
	DepthStencil clearDepthStencil(1.0, 0);
	Rect2D area(0, 0, windowWidth, windowHeight);
	Viewport viewport(0, 0, windowWidth, windowHeight, 0, 1);

	device.BeginRenderPass(area, clearColor, clearDepthStencil);

	device.SetViewport(viewport);
	device.SetScissor(area);

	// m_ColorCubeNode
	SetShader(m_ColorShader);
	BindMaterial(m_ColorMat);
	DrawRenderNode(m_ColorCubeNode);

	// m_TexCubeNode
	SetShader(m_TexShader);
	BindMaterial(m_TexMat);
	DrawRenderNode(m_TexCubeNode);

	// m_LitSphereNode
	SetShader(m_LitShader);
	BindMaterial(m_LitMat);
	DrawRenderNode(m_LitSphereNode);

	DrawImgui();

	device.EndRenderPass();

	device.EndCommandBuffer();
}

void Triangle::PrepareResources()
{
	// Mesh
	{
		// 自定义顶点buffer的结构和数据
		m_CustomCubeMesh = CreateMesh("CustomCubeMesh");
		m_CustomCubeMesh->SetVertexChannels({ kVertexPosition, kVertexColor });
		std::vector<float> vertices = {
			-1.0f, -1.0f,  1.0f , 1.0f, 0.0f, 0.0f  ,
			1.0f, -1.0f,  1.0f , 0.0f, 1.0f, 0.0f  ,
			1.0f,  1.0f,  1.0f , 0.0f, 0.0f, 1.0f  ,
			-1.0f,  1.0f,  1.0f , 0.0f, 0.0f, 0.0f  ,
			-1.0f, -1.0f, -1.0f , 1.0f, 0.0f, 0.0f  ,
			1.0f, -1.0f, -1.0f , 0.0f, 1.0f, 0.0f  ,
			1.0f,  1.0f, -1.0f , 0.0f, 0.0f, 1.0f  ,
			-1.0f,  1.0f, -1.0f , 0.0f, 0.0f, 0.0f  ,
		};
		std::vector<uint32_t> indices = {
			0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 4,5,1, 1,0,4, 3,2,6, 6,7,3,
		};
		m_CustomCubeMesh->SetVertices(vertices);
		m_CustomCubeMesh->SetIndices(indices);
		m_CustomCubeMesh->UploadToGPU();

		m_TexCubeMesh = CreateMesh("CubeMesh");
		m_TexCubeMesh->SetVertexChannels({ kVertexPosition, kVertexTexcoord0 });
		m_TexCubeMesh->LoadFromFile(AssetPath + "models/cube.obj");
		m_TexCubeMesh->UploadToGPU();

		m_SphereMesh = CreateMesh("SphereMesh");
		m_SphereMesh->LoadFromFile(AssetPath + "models/sphere.obj");
		m_SphereMesh->UploadToGPU();
	}

	// Texture
	{
		m_Crate01Tex = CreateTexture("Crate01Tex");
		m_Crate01Tex->LoadFromFile(AssetPath + "textures/crate01_color_height_rgba.ktx");

		m_Crate02Tex = CreateTexture("Crate02Tex");
		m_Crate02Tex->LoadFromFile(AssetPath + "textures/crate02_color_height_rgba.ktx");

		m_MetalplateTex = CreateTexture("MetalplateTex");
		m_MetalplateTex->LoadFromFile(AssetPath + "textures/metalplate_nomips_rgba.ktx");
	}

	// Shader
	{
		m_ColorShader = CreateShader("ColorShader");
		m_ColorShader->LoadSPV(AssetPath + "shaders/triangle/Color.vert.spv", AssetPath + "shaders/triangle/Color.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", GpuParameters::kUniformDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		m_ColorShader->CreateGpuProgram(parameters);

		RenderState renderState;

		m_ColorShader->SetRenderState(renderState);
	}
	{
		m_TexShader = CreateShader("TexShader");
		m_TexShader->LoadSPV(AssetPath + "shaders/triangle/Tex.vert.spv", AssetPath + "shaders/triangle/Tex.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::TextureParameter texture("BaseTexture", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.textureParameters.push_back(texture);
		}
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", GpuParameters::kUniformDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		m_TexShader->CreateGpuProgram(parameters);

		RenderState renderState;

		m_TexShader->SetRenderState(renderState);
	}
	{
		m_LitShader = CreateShader("LitShader");
		m_LitShader->LoadSPV(AssetPath + "shaders/triangle/Lit.vert.spv", AssetPath + "shaders/triangle/Lit.frag.spv");

		GpuParameters parameters;
		{
			GpuParameters::UniformParameter uniform("PerMaterial", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
			uniform.valueParameters.emplace_back("DiffuseAlbedo", GpuParameters::kUniformDataFloat4);
			uniform.valueParameters.emplace_back("FresnelR0", GpuParameters::kUniformDataFloat3);
			uniform.valueParameters.emplace_back("Roughness", GpuParameters::kUniformDataFloat1);
			uniform.valueParameters.emplace_back("MatTransform", GpuParameters::kUniformDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		{
			GpuParameters::TextureParameter texture("BaseTexture", 1, VK_SHADER_STAGE_FRAGMENT_BIT);
			parameters.textureParameters.push_back(texture);
		}
		{
			GpuParameters::UniformParameter uniform("PerDraw", 0, VK_SHADER_STAGE_VERTEX_BIT);
			uniform.valueParameters.emplace_back("ObjectToWorld", GpuParameters::kUniformDataFloat4x4);
			parameters.uniformParameters.push_back(uniform);
		}
		m_LitShader->CreateGpuProgram(parameters);

		RenderState renderState;

		m_LitShader->SetRenderState(renderState);

		//m_LitShader->AddSpecializationConstant(0, 1);
		//m_LitShader->AddSpecializationConstant(1, 1);
		//// todo, shader中 0<0 会有问题
		//m_LitShader->AddSpecializationConstant(2, 1);
	}

	// Material
	{
		m_ColorMat = CreateMaterial("SimpleColorMat");
		m_ColorMat->SetShader(m_ColorShader);

		m_TexMat = CreateMaterial("TexMat");
		m_TexMat->SetShader(m_TexShader);
		m_TexMat->SetTexture("BaseTexture", m_Crate01Tex);

		m_LitMat = CreateMaterial("LitMat");
		m_LitMat->SetShader(m_LitShader);
		m_LitMat->SetFloat4("DiffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_LitMat->SetFloat3("FresnelR0", 0.3f, 0.3f, 0.3f);
		m_LitMat->SetFloat("Roughness", 0.3f);
		m_LitMat->SetTexture("BaseTexture", m_Crate02Tex);
	}

	// RenderNode
	{
		m_ColorCubeNode = CreateRenderNode("ColorCubeNode");
		m_ColorCubeNode->SetMesh(m_CustomCubeMesh);
		m_ColorCubeNode->SetMaterial(m_ColorMat);
		m_ColorCubeNode->GetTransform().Scale(0.1f, 0.1f, 0.1f).Translate(0.0f, 0.6f, 0.0f);

		m_TexCubeNode = CreateRenderNode("TexCubeNode");
		m_TexCubeNode->SetMesh(m_TexCubeMesh);
		m_TexCubeNode->SetMaterial(m_TexMat);
		m_TexCubeNode->GetTransform().Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, 0.3f, 0.0f);

		m_LitSphereNode = CreateRenderNode("LitSphereNode");
		m_LitSphereNode->SetMesh(m_SphereMesh);
		m_LitSphereNode->SetMaterial(m_LitMat);
		m_LitSphereNode->GetTransform().Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, 0.0f, 0.0f);
	}
}
