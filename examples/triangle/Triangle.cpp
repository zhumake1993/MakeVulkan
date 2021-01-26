#include "Triangle.h"
#include "DeviceProperties.h"
#include "GfxTypes.h"
#include "GfxDevice.h"
#include "Settings.h"

#include "Mesh.h"
#include "Texture.h"

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
		m_CubeMesh = CreateMesh();
		m_CubeMesh->LoadFromFile(AssetPath + "models/cube.obj");
		m_CubeMesh->UploadToGPU();

		m_CustomCubeMesh = CreateMesh();
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

		m_SphereMesh = CreateMesh();
		m_SphereMesh->LoadFromFile(AssetPath + "models/sphere.obj");
		m_SphereMesh->UploadToGPU();
	}

	// Texture
	{
		m_Crate01Tex = CreateTexture();
		m_Crate01Tex->LoadFromFile(AssetPath + "textures/crate01_color_height_rgba.ktx");

		m_Crate02Tex = CreateTexture();
		m_Crate02Tex->LoadFromFile(AssetPath + "textures/crate02_color_height_rgba.ktx");

		m_MetalplateTex = CreateTexture();
		m_MetalplateTex->LoadFromFile(AssetPath + "textures/metalplate_nomips_rgba.ktx");
	}
}
