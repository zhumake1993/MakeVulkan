#include "VKSurface.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKInstance.h"

#if defined(VK_USE_PLATFORM_WIN32_KHR)

VKSurfacce::VKSurfacce(VKInstance* vkInstance, HINSTANCE windowInstance, HWND windowHandle) :
	instance(vkInstance->instance)
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = windowInstance;
	surfaceCreateInfo.hwnd = windowHandle;

	VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

VKSurfacce::VKSurfacce(VKInstance* vkInstance, ANativeWindow* window) :
	instance(vkInstance->instance)
{
	VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.window = window;

	VK_CHECK_RESULT(vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
}

#endif

VKSurfacce::~VKSurfacce()
{
	if (instance != VK_NULL_HANDLE && surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
		surface = VK_NULL_HANDLE;
	}

	instance = VK_NULL_HANDLE;
}