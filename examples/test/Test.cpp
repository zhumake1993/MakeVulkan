#include "Test.h"

void ConfigGlobalSettings() {
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

Test::Test()
{
}

Test::~Test()
{
}

// 入口

#ifdef _WIN32

#if USE_VISUAL_LEAK_DETECTOR
#include "vld.h"
#endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	global::windowInstance = hInstance;

	ConfigGlobalSettings();

	application = new Application(new Test());
	application->Init();
	application->Run();
	application->CleanUp();
	RELEASE(application);

	system("PAUSE");
	return 0;
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void android_main(android_app* state)
{
	androidApp = state;

	ConfigGlobalSettings();

	application = new Application(new Test());
	state->userData = application;
	state->onAppCmd = HandleAppCommand;
	state->onInputEvent = HandleAppInput;
	application->Run();
	application->CleanUp();
	RELEASE(application);
}

#endif