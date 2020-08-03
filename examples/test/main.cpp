#include <iostream>
#include "VulkanBase.h"
#include "Tools.h"

class VulkanExample : public VulkanBase
{

public:

	VulkanExample() {
		
		// 添加单独的实例级层
		// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
		// Note that on Android this layer requires at least NDK r20
#if defined(_WIN32)
		// 目前我用的NDK r19
		global::enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

		// 添加单独的实例级拓展
		// 这个暂时搁置
		//global::enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		global::enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
		global::enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		global::enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

		global::enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	~VulkanExample() {

	}
};

VulkanExample *vulkanExample;

#ifdef _WIN32

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (vulkanExample != nullptr)
	{
		vulkanExample->HandleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	vulkanExample = new VulkanExample();

	vulkanExample->SetupConsole();
	vulkanExample->SetupWindow(hInstance, WndProc);
	vulkanExample->Init();
	vulkanExample->Prepare();
	vulkanExample->Run();

	system("PAUSE");

	delete(vulkanExample);
	return 0;
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void android_main(android_app* state)
{
	vulkanExample = new VulkanExample();

	androidApp = state;
	androidApp->userData = vulkanExample;
	androidApp->onAppCmd = VulkanExample::HandleAppCommand;
	androidApp->onInputEvent = VulkanExample::HandleAppInput;
	vulkanExample->Run();

	delete(vulkanExample);
}

#endif