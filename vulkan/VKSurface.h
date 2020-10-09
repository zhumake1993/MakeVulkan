#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKInstance;

struct VKSurface : public NonCopyable
{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	VKSurface(VKInstance* vkInstance, HINSTANCE windowInstance, HWND windowHandle);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	VKSurface(VKInstance* vkInstance, ANativeWindow* window);
#endif
	~VKSurface();

	VkSurfaceKHR surface = VK_NULL_HANDLE;

private:

	VkInstance instance = VK_NULL_HANDLE;
};