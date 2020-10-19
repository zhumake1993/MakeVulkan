#include "Common.h"

#ifdef _WIN32
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

android_app* androidApp;

#endif

namespace global
{

	// display
#if defined(_WIN32)
	HINSTANCE windowInstance;
	HWND windowHandle;
	std::string consoleTitle = "Console Title";
	std::string windowClassName = "Window Class Name";
	std::string windowTitleName = "Window Title Name";
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
#endif
	uint32_t windowWidth = 1280;
	uint32_t windowHeight = 720;

#if defined(_WIN32)
	std::string AssetPath = "C:/Users/1/Desktop/MakeVulkan/data/";
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	std::string AssetPath = "";
#endif

}