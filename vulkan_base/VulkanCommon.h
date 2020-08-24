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
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // OpenGL使用的默认深度范围是[-1,1]
#define GLM_FORCE_LEFT_HANDED // 强制使用左手坐标系，注意，定义GLM_LEFT_HANDED宏是没用的
#include "../external/glm-0.9.9.7/glm/glm/glm.hpp"
#include "../external/glm-0.9.9.7/glm/glm/gtc/matrix_transform.hpp"

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

#if defined(_WIN32)

#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define KEY_Q 0x51
#define KEY_E 0x45

struct KeyboardInput
{
	bool key_W = false;
	bool key_S = false;
	bool key_A = false;
	bool key_D = false;
	bool key_Q = false;
	bool key_E = false;
	bool key_MouseLeft = false;
	bool key_MouseRight = false;
	glm::vec2 pos = glm::vec2(0.0f, 0.0f);
	glm::vec2 oldPos = glm::vec2(0.0f, 0.0f);
};

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

struct KeyboardInput
{
	bool touch = false;
	glm::vec2 pos = glm::vec2(0.0f, 0.0f);
	glm::vec2 oldPos = glm::vec2(0.0f, 0.0f);
};

#endif