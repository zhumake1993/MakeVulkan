#include "VKSurface.h"
#include "VulkanTools.h"

VKSurface::VKSurface(VkInstance vkInstance) :
	instance(vkInstance)
{
#if defined(_WIN32)

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = windowInstance;
	surfaceCreateInfo.hwnd = windowHandle;

	VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.window = androidApp->window;

	VK_CHECK_RESULT(vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));

#endif
	
}

VKSurface::~VKSurface()
{
	if (instance != VK_NULL_HANDLE && surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
		surface = VK_NULL_HANDLE;
	}
}