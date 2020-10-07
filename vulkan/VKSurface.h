#pragma once

#include "Common.h"
#include "NonCopyable.h"

class VKInstance;

struct VKSurfacce : public NonCopyable
{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	VKSurfacce(VKInstance* vkInstance, HINSTANCE windowInstance, HWND windowHandle);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	VKSurfacce(VKInstance* vkInstance, ANativeWindow* window);
#endif
	~VKSurfacce();

	VkSurfaceKHR surface = VK_NULL_HANDLE;

private:

	VkInstance instance = VK_NULL_HANDLE;
};