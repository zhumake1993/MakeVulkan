#pragma once

#ifdef _WIN32

#include <windows.h>
#include <vulkan/vulkan.h>

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

#include <android/log.h>
#include <android_native_app_glue.h>
#include <vulkan_wrapper.h>
#include <sys/system_properties.h>
#include <android/asset_manager.h>

// Global reference to android application object
extern android_app* androidApp;

#endif

#include <string>
#include <vector>
#include <assert.h>
#include <memory>

// 全局设置

namespace global
{

	// window
#if defined(_WIN32)
	extern std::string consoleTitle;
	extern std::string windowClassName;
	extern std::string windowTitleName;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
#endif
	extern uint32_t windowWidth;
	extern uint32_t windowHeight;

	// Vulkan Instance
	extern std::string applicationName;
	extern uint32_t applicationVersion;
	extern std::string engineName;
	extern uint32_t engineVersion;
	extern uint32_t apiVersion;
	extern std::vector<const char*> enabledInstanceLayers;
	extern std::vector<const char*> enabledInstanceExtensions;

	// Vulkan Device
	extern VkPhysicalDeviceFeatures enabledDeviceFeatures;
	extern std::vector<const char*> enabledDeviceExtensions;

	// Frame
	extern size_t frameResourcesCount;
}
