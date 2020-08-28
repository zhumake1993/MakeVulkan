#include "VulkanSurface.h"
#include "VulkanInstance.h"
#include "Tools.h"

#if defined(VK_USE_PLATFORM_WIN32_KHR)

VulkanSurface::VulkanSurface(VulkanInstance* vulkanInstance, HINSTANCE windowInstance, HWND windowHandle):
	m_VulkanInstance(vulkanInstance)
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = windowInstance;
	surfaceCreateInfo.hwnd = windowHandle;

	VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(vulkanInstance->m_Instance, &surfaceCreateInfo, nullptr, &m_Surface));
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

VulkanSurface::VulkanSurface(VulkanInstance* vulkanInstance, ANativeWindow* window)
{
	VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.window = window;

	VK_CHECK_RESULT(vkCreateAndroidSurfaceKHR(vulkanInstance->m_Instance, &surfaceCreateInfo, nullptr, &m_Surface));
}

#endif

VulkanSurface::~VulkanSurface()
{
}

void VulkanSurface::CleanUp()
{
	if (m_VulkanInstance && m_VulkanInstance->m_Instance != VK_NULL_HANDLE && m_Surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(m_VulkanInstance->m_Instance, m_Surface, nullptr);
		m_Surface = VK_NULL_HANDLE;
	}
}

bool VulkanSurface::SupportPresent(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
	VkBool32 supportPresent;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, m_Surface, &supportPresent);
	return supportPresent == VK_TRUE;
}
