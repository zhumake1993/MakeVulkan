#include "GlobalSettings.h"
#include "Log.h"

#ifdef _WIN32
#if USE_VISUAL_LEAK_DETECTOR
#include "vld.h"
#endif
#endif

GlobalSettings::GlobalSettings()
{
#if defined(_WIN32)
	consoleTitle = "Console Title";
	windowClassName = "Window Class Name";
	windowTitleName = "Window Title Name";
	windowWidth = 1280;
	windowHeight = 720;
#endif

#if defined(_WIN32)
	assetPath = "D:/Vulkan/MakeVulkan/data/";
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	assetPath = "";
#endif

	// Instance
	applicationName = "Application Name";
	applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	engineName = "Engine Name";
	engineVersion = VK_MAKE_VERSION(1, 0, 0);
	apiVersion = VK_API_VERSION_1_0;
#if defined(_WIN32)
	enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation"); // 安卓上面使用这个layer有点问题，待研究
#endif
	enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
	enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

	// Device
	enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

GlobalSettings::~GlobalSettings()
{
}

void GlobalSettings::Print()
{
	LOG("[GlobalSettings]\n");

#if defined(_WIN32)
	LOG("window extent: %d x %d\n:", static_cast<int>(windowWidth), static_cast<int>(windowHeight));
#endif

	LOG("enabled instance layer ( %d ):", static_cast<int>(enabledInstanceLayers.size()));
	for (size_t i = 0; i < enabledInstanceLayers.size(); i++)
	{
		LOG(" %s", enabledInstanceLayers[i]);
	}
	LOG("\n");

	LOG("enabled instance extensions ( %d ):", static_cast<int>(enabledInstanceExtensions.size()));
	for (size_t i = 0; i < enabledInstanceExtensions.size(); i++)
	{
		LOG(" %s", enabledInstanceExtensions[i]);
	}
	LOG("\n");

	LOG("enabled device extensions ( %d ):", static_cast<int>(enabledDeviceExtensions.size()));
	for (size_t i = 0; i < enabledDeviceExtensions.size(); i++)
	{
		LOG(" %s", enabledDeviceExtensions[i]);
	}
	LOG("\n");

	LOG("memoryAlignment: %d\n:", static_cast<int>(memoryAlignment));
	LOG("memoryBlockSize: %d\n:", static_cast<int>(memoryBlockSize));
}

GlobalSettings gGlobalSettings;

GlobalSettings & GetGlobalSettings()
{
	return gGlobalSettings;
}
