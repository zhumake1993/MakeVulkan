#include "VKSwapChain.h"
#include "Platforms.h"
#include "VKTools.h"
#include "DeviceProperties.h"
#include "GlobalSettings.h"
#include "Tools.h"
#include "Log.h"

void CheckSurfaceStats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	auto& dp = GetDeviceProperties();

	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &dp.surfaceCapabilities));

	uint32_t formatsCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, nullptr));
	dp.surfaceFormats.resize(formatsCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, dp.surfaceFormats.data()));

	uint32_t presentModesCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr));
	dp.presentModes.resize(presentModesCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, dp.presentModes.data()));
}

uint32_t GetSwapChainImageNum()
{
	auto& dp = GetDeviceProperties();

	uint32_t imageCount = dp.surfaceCapabilities.minImageCount + 1;
	if (dp.surfaceCapabilities.maxImageCount > 0 && imageCount > dp.surfaceCapabilities.maxImageCount)
	{
		imageCount = dp.surfaceCapabilities.maxImageCount;
	}

	return imageCount;
}

VkSurfaceFormatKHR GetSwapChainSurfaceFormat()
{
	auto& dp = GetDeviceProperties();

	// If the list contains only one entry with undefined format
	// it means that there are no preferred surface formats and any can be chosen
	if (dp.surfaceFormats.size() == 1 && dp.surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	// Check if list contains most widely used R8 G8 B8 A8 format
	// with nonlinear color space
	for (VkSurfaceFormatKHR &surfaceformat : dp.surfaceFormats)
	{
		if (surfaceformat.format == VK_FORMAT_R8G8B8A8_UNORM)
		{
			return surfaceformat;
		}
	}

	// Return the first format from the list
	return dp.surfaceFormats[0];
}

VkExtent2D GetSwapChainExtent()
{
	auto& dp = GetDeviceProperties();

	// Special value of surface extent is width == height == -1
	// If this is so we define the size by ourselves but it must fit within defined confines
	if (dp.surfaceCapabilities.currentExtent.width == -1)
	{
#ifdef _WIN32
		VkExtent2D swap_chain_extent = { GetGlobalSettings().windowWidth, GetGlobalSettings().windowHeight };
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		VkExtent2D swap_chain_extent = { 0, 0 };
#endif

		if (swap_chain_extent.width < dp.surfaceCapabilities.minImageExtent.width)
			swap_chain_extent.width = dp.surfaceCapabilities.minImageExtent.width;

		if (swap_chain_extent.height < dp.surfaceCapabilities.minImageExtent.height)
			swap_chain_extent.height = dp.surfaceCapabilities.minImageExtent.height;

		if (swap_chain_extent.width > dp.surfaceCapabilities.maxImageExtent.width)
			swap_chain_extent.width = dp.surfaceCapabilities.maxImageExtent.width;

		if (swap_chain_extent.height > dp.surfaceCapabilities.maxImageExtent.height)
			swap_chain_extent.height = dp.surfaceCapabilities.maxImageExtent.height;

		return swap_chain_extent;
	}

	// Most of the cases we define size of the swap_chain images equal to current window's size
	return dp.surfaceCapabilities.currentExtent;
}

VkImageUsageFlags GetSwapChainUsageFlags()
{
	auto& dp = GetDeviceProperties();

	// todo：swap chain image的内容可能是直接copy上去的
	//// Enable transfer source on swap chain images if supported
	//if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
	//	swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	//}

	//// Enable transfer destination on swap chain images if supported
	//if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
	//	swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	//}

	// Color attachment flag must always be supported
	// We can define other usage flags but we always need to check if they are supported
	if (dp.surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	LOGE("VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT image usage is not supported by the swap chain!");
	return 0;
}

VkSurfaceTransformFlagBitsKHR GetSwapChainTransform()
{
	auto& dp = GetDeviceProperties();

	// Sometimes images must be transformed before they are presented (i.e. due to device's orienation
	// being other than default orientation)
	// If the specified transform is other than current transform, presentation engine will transform image
	// during presentation operation; this operation may hit performance on some platforms
	// Here we don't want any transformations to occur so if the identity transform is supported use it
	// otherwise just use the same transform as current transform
	if (dp.surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		return dp.surfaceCapabilities.currentTransform;
}

VkPresentModeKHR GetSwapChainPresentMode()
{
	auto& dp = GetDeviceProperties();

	// MAILBOX is the lowest latency V-Sync enabled mode (something like triple-buffering) so use it if available
	for (VkPresentModeKHR &presentMode : dp.presentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return presentMode;
	}

	// IMMEDIATE mode allows us to display frames in a V-Sync independent manner so it can introduce screen tearing
	// But this mode is the best for benchmarking purposes if we want to check the real number of FPS
	for (VkPresentModeKHR &presentMode : dp.presentModes)
	{
		if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			return presentMode;
	}

	// FIFO present mode is always available
	for (VkPresentModeKHR &presentMode : dp.presentModes)
	{
		if (presentMode == VK_PRESENT_MODE_FIFO_KHR)
			return presentMode;
	}

	LOGE("FIFO present mode is not supported by the swap chain!");

	return VK_PRESENT_MODE_MAX_ENUM_KHR;
}

vk::VKSwapChain::VKSwapChain(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, int selectedQueueFamilyIndex)
	: m_Instance(instance)
	, m_Device(device)
{
	// Surface

#if defined(_WIN32)

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = platform::GetWindowInstance();
	surfaceCreateInfo.hwnd = platform::GetWindowHandle();

	VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &m_Surface));

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.window = platform::GetAndroidApp()->window;

	VK_CHECK_RESULT(vkCreateAndroidSurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &m_Surface));

#endif

	// 检查对present的支持

	VkBool32 supportPresent;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, selectedQueueFamilyIndex, m_Surface, &supportPresent);
	ASSERT(supportPresent);

	// SwapChain

	auto& gs = GetGlobalSettings();

	CheckSurfaceStats(physicalDevice, m_Surface);

	m_ImageNum = GetSwapChainImageNum();
	m_SurfaceFormat = GetSwapChainSurfaceFormat();
	m_Extent = GetSwapChainExtent();
	m_Usage = GetSwapChainUsageFlags();
	m_Transform = GetSwapChainTransform();
	m_PresentMode = GetSwapChainPresentMode();

	// 主要跟全屏有关，这里我们不使用
	VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;

	if (m_Extent.width == 0 || m_Extent.height == 0)
	{
		// 有时候（窗口最小化）会出现这种情况
		// 直接退出
		LOGE("extent is 0");
	}

#if defined(_WIN32)
	// PC上屏幕分辨率不应该发生变化
	if (gs.windowWidth != m_Extent.width || gs.windowHeight != m_Extent.height)
	{
		LOGE("Resolution not match. prefered extent: %d x %d, actual extent: %d x %d", gs.windowWidth, gs.windowHeight, m_Extent.width, m_Extent.height);
	}
#endif

	VkSwapchainCreateInfoKHR swapchainCI = {};
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = nullptr;
	swapchainCI.flags = 0;
	swapchainCI.surface = m_Surface;
	swapchainCI.minImageCount = m_ImageNum;
	swapchainCI.imageFormat = m_SurfaceFormat.format;
	swapchainCI.imageColorSpace = m_SurfaceFormat.colorSpace;
	swapchainCI.imageExtent = m_Extent;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageUsage = m_Usage;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.pQueueFamilyIndices = nullptr;
	swapchainCI.preTransform = m_Transform;
	swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCI.presentMode = m_PresentMode;
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.oldSwapchain = oldSwapChain;

	VK_CHECK_RESULT(vkCreateSwapchainKHR(m_Device, &swapchainCI, nullptr, &m_SwapChain));

	uint32_t imageCount = 0;
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr));
	m_SwapChainImages.resize(imageCount);
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data()));

	// 创建SwapChain时提供的参数是minImageCount，实际的image数量可能不一样
	// 尤其是使用VK_PRESENT_MODE_MAILBOX_KHR的时候
	m_ImageNum = imageCount;

	m_SwapChainImageViews.resize(imageCount);
	for (uint32_t i = 0; i < imageCount; i++)
	{
		VkImageViewCreateInfo colorAttachmentView = {};
		colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorAttachmentView.pNext = nullptr;
		colorAttachmentView.flags = 0;
		colorAttachmentView.image = m_SwapChainImages[i];
		colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentView.format = m_SurfaceFormat.format;
		colorAttachmentView.components = {
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorAttachmentView.subresourceRange.baseMipLevel = 0;
		colorAttachmentView.subresourceRange.levelCount = 1;
		colorAttachmentView.subresourceRange.baseArrayLayer = 0;
		colorAttachmentView.subresourceRange.layerCount = 1;

		VK_CHECK_RESULT(vkCreateImageView(device, &colorAttachmentView, nullptr, &m_SwapChainImageViews[i]));
	}
}

vk::VKSwapChain::~VKSwapChain()
{
	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i) {
		vkDestroyImageView(m_Device, m_SwapChainImageViews[i], nullptr);
	}
	m_SwapChainImageViews.clear();

	// Swap Chain Image会随着SwapChain的销毁而自动销毁
	m_SwapChainImages.clear();

	vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);

	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
}

void vk::VKSwapChain::Print()
{
	LOG("[VKSwapChain]\n");

	LOG("image num: %d\n", m_ImageNum);
	LOG("surface format: %d %d\n", m_SurfaceFormat.format, m_SurfaceFormat.colorSpace);
	LOG("extent: %d %d\n", m_Extent.width, m_Extent.height);
	LOG("usage: %d\n", m_Usage);
	LOG("transform: %d\n", m_Transform);
	LOG("presentMode: %d\n", m_PresentMode);

	LOG("\n");
}
