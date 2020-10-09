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
#include <unordered_map>
#include <forward_list>
#include <assert.h>
#include <memory>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // OpenGLʹ�õ�Ĭ����ȷ�Χ��[-1,1]
#define GLM_FORCE_LEFT_HANDED // ǿ��ʹ����������ϵ��ע�⣬����GLM_LEFT_HANDED����û�õ�
#include "glm-0.9.9.7/glm/glm/glm.hpp"
#include "glm-0.9.9.7/glm/glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"

// ʹ��Visual Leak Detector������PC��
#ifdef _WIN32
#define USE_VISUAL_LEAK_DETECTOR 0
#endif

namespace global
{

	// display
#if defined(_WIN32)
	extern HINSTANCE windowInstance;
	extern HWND windowHandle;
	extern std::string consoleTitle;
	extern std::string windowClassName;
	extern std::string windowTitleName;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
#endif
	extern uint32_t windowWidth;
	extern uint32_t windowHeight;

	// Frame
	extern uint32_t frameResourcesCount;

	extern std::string AssetPath;

}

struct PassUniform {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;
	alignas(16) glm::vec4 lightPos;
};

struct ObjectUniform {
	alignas(16) glm::mat4 world;
};