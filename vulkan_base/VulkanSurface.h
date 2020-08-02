#pragma once

#include "VulkanCommon.h"

class VulkanInstance;

class VulkanSurface
{

public:

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	VulkanSurface(VulkanInstance* vulkanInstance, HINSTANCE windowInstance, HWND windowHandle);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	VulkanSurface(VulkanInstance* vulkanInstance, ANativeWindow* window);
#endif
	~VulkanSurface();

	void CleanUp();
	bool SupportPresent(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);

public:

	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

private:

	VulkanInstance* m_VulkanInstance = nullptr;

};