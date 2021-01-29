#include "Triangle.h"
#include "DeviceProperties.h"
#include "GfxTypes.h"
#include "GfxDevice.h"
#include "Settings.h"

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "RenderNode.h"

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

	// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
	// Note that on Android this layer requires at least NDK r20
#if defined(_WIN32)
	// 目前我用的NDK r19
	dp.enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// 添加InstanceExtension

	// 这个暂时搁置
	//global::enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

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
	PrepareResources();
}

void Triangle::Release()
{
}

void Triangle::Update()
{
	auto& device = GetGfxDevice();

	device.BeginCommandBuffer();

	Color clearColor;
	DepthStencil clearDepthStencil;
	Rect2D area(0, 0, windowWidth, windowHeight);
	Viewport viewport(windowWidth, windowHeight);

	device.BeginRenderPass(area, clearColor, clearDepthStencil);

	device.SetViewport(viewport);
	device.SetScissor(area);















	device.EndRenderPass();

	device.EndCommandBuffer();
}

void Triangle::PrepareResources()
{
	// Mesh
	{
		m_CubeMesh = CreateMesh("CubeMesh");
		m_CubeMesh->LoadFromFile(AssetPath + "models/cube.obj");
		m_CubeMesh->UploadToGPU();

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

		UniformBufferLayout layout0("Global", 0);
		layout0.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "MatrixView"));
		layout0.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "MatrixProj"));

		UniformBufferLayout layout1("PerDraw", 1);
		layout1.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "ObjectToWorld"));

		m_ColorShader->SetUniformBufferDesc(UniformBufferDesc(std::vector<UniformBufferLayout>{ layout0, layout1 }));
	}
	{
		m_LitShader = CreateShader("LitShader");
		m_LitShader->LoadSPV(AssetPath + "shaders/triangle/Lit.vert.spv", AssetPath + "shaders/triangle/Lit.frag.spv");

		UniformBufferLayout layout0("Global", 0);
		layout0.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "MatrixView"));
		layout0.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "MatrixProj"));
		layout0.Add(UniformBufferElement(kUniformDataTypeFloat4, "EyePos"));

		UniformBufferLayout layout1("PerMaterial", 1);
		layout1.Add(UniformBufferElement(kUniformDataTypeFloat4, "DiffuseAlbedo"));
		layout1.Add(UniformBufferElement(kUniformDataTypeFloat3, "FresnelR0"));
		layout1.Add(UniformBufferElement(kUniformDataTypeFloat1, "Roughness"));
		layout1.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "MatTransform"));

		UniformBufferLayout layout2("PerDraw", 2);
		layout2.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "ObjectToWorld"));

		m_LitShader->SetUniformBufferDesc(UniformBufferDesc(std::vector<UniformBufferLayout>{ layout0, layout1, layout2 }));
		m_LitShader->SetTextureDesc({ "BaseTexture" });

		//m_LitShader->AddSpecializationConstant(0, 1);
		//m_LitShader->AddSpecializationConstant(1, 1);
		//// todo, shader中 0<0 会有问题
		//m_LitShader->AddSpecializationConstant(2, 1);
	}

	// Material
	{
		m_Crate01Mat = CreateMaterial("Crate01Mat");
		m_Crate01Mat->SetShader(m_LitShader);
		m_Crate01Mat->SetFloat4("DiffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_Crate01Mat->SetFloat3("FresnelR0", 0.3f, 0.3f, 0.3f);
		m_Crate01Mat->SetFloat("Roughness", 0.3f);
		m_Crate01Mat->SetTextures("BaseTexture", m_Crate01Tex);
		//CreateVKPipeline(m_Crate01Mat, m_Home->GetVertexDescription());

		m_Crate02Mat = CreateMaterial("Crate02Mat");
		m_Crate02Mat->SetShader(m_LitShader);
		m_Crate02Mat->SetFloat4("DiffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_Crate02Mat->SetFloat3("FresnelR0", 0.3f, 0.3f, 0.3f);
		m_Crate02Mat->SetFloat("Roughness", 0.3f);
		m_Crate02Mat->SetTextures("BaseTexture", m_Crate02Tex);
		//CreateVKPipeline(m_Crate02Mat, m_Home->GetVertexDescription());

		m_ColorMat = CreateMaterial("SimpleColorMat");
		m_ColorMat->SetShader(m_ColorShader);
		//CreateVKPipeline(m_SimpleColorMat, m_SimpleCube->GetVertexDescription());

		m_MetalplateMat = CreateMaterial("MetalplateMat");
		m_MetalplateMat->SetShader(m_LitShader);
		m_MetalplateMat->SetFloat4("DiffuseAlbedo", 1.0f, 1.0f, 1.0f, 1.0f);
		m_MetalplateMat->SetFloat3("FresnelR0", 0.3f, 0.3f, 0.3f);
		m_MetalplateMat->SetFloat("Roughness", 0.3f);
		m_MetalplateMat->SetTextures("BaseTexture", m_MetalplateTex);
		//CreateVKPipeline(m_MetalplateMat, m_Quad->GetVertexDescription());
	}

	// RenderNode
	{
		m_CubeNode = CreateRenderNode("CubeNode");
		m_CubeNode->SetMesh(m_CubeMesh);
		m_CubeNode->SetMaterial(m_Crate01Mat);
		m_CubeNode->GetTransform().Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, 0.3f, 0.3f);

		m_SphereNode = CreateRenderNode("SphereNode");
		m_SphereNode->SetMesh(m_SphereMesh);
		m_SphereNode->SetMaterial(m_Crate02Mat);
		m_SphereNode->GetTransform().Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, 0.3f, 0.0f);

		m_ColorCubeNode = CreateRenderNode("ColorCubeNode");
		m_ColorCubeNode->SetMesh(m_CustomCubeMesh);
		m_ColorCubeNode->SetMaterial(m_ColorMat);
		m_ColorCubeNode->GetTransform().Scale(0.1f, 0.1f, 0.1f).Translate(0.0f, 0.6f, 0.0f);
	}
}
