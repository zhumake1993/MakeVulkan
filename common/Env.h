#pragma once

// ƽ̨����

#ifdef _WIN32

#include <windows.h>
#include <vulkan/vulkan.h>

extern HINSTANCE windowInstance;
extern HWND windowHandle;

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

#include <android/log.h>
#include <android_native_app_glue.h>
#include <vulkan_wrapper.h>
#include <sys/system_properties.h>
#include <android/asset_manager.h>

// Global reference to android application object
extern android_app* androidApp;

#endif

// ����STL

#include <string>
#include <vector>
#include <unordered_map>
#include <forward_list>
#include <list>
#include <assert.h>
#include <memory>
#include <chrono>

// GLM

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // OpenGLʹ�õ�Ĭ����ȷ�Χ��[-1,1]
#define GLM_FORCE_LEFT_HANDED // ǿ��ʹ����������ϵ��ע�⣬����GLM_LEFT_HANDED����û�õ�
#include "glm-0.9.9.7/glm/glm/glm.hpp"
#include "glm-0.9.9.7/glm/glm/gtc/matrix_transform.hpp"

// imgui

#include "imgui/imgui.h"