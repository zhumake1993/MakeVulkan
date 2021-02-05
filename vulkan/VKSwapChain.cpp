#include "VKSwapChain.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"
#include "Settings.h"

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

uint32_t GetSwapChainNumImages()
{
	auto& dp = GetDeviceProperties();

	uint32_t imageCount = dp.surfaceCapabilities.minImageCount + 1;
	if ((dp.surfaceCapabilities.maxImageCount > 0) &&
		(imageCount > dp.surfaceCapabilities.maxImageCount)) {
		imageCount = dp.surfaceCapabilities.maxImageCount;
	}

	return imageCount;
}

VkSurfaceFormatKHR GetSwapChainFormat()
{
	auto& dp = GetDeviceProperties();

	// If the list contains only one entry with undefined format
	// it means that there are no preferred surface formats and any can be chosen
	if ((dp.surfaceFormats.size() == 1) &&
		(dp.surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
		return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	// Check if list contains most widely used R8 G8 B8 A8 format
	// with nonlinear color space
	for (VkSurfaceFormatKHR &surfaceformat : dp.surfaceFormats) {
		if (surfaceformat.format == VK_FORMAT_R8G8B8A8_UNORM) {
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
	if (dp.surfaceCapabilities.currentExtent.width == -1) {
		VkExtent2D swap_chain_extent = { windowWidth, windowHeight };
		if (swap_chain_extent.width < dp.surfaceCapabilities.minImageExtent.width) {
			swap_chain_extent.width = dp.surfaceCapabilities.minImageExtent.width;
		}
		if (swap_chain_extent.height < dp.surfaceCapabilities.minImageExtent.height) {
			swap_chain_extent.height = dp.surfaceCapabilities.minImageExtent.height;
		}
		if (swap_chain_extent.width > dp.surfaceCapabilities.maxImageExtent.width) {
			swap_chain_extent.width = dp.surfaceCapabilities.maxImageExtent.width;
		}
		if (swap_chain_extent.height > dp.surfaceCapabilities.maxImageExtent.height) {
			swap_chain_extent.height = dp.surfaceCapabilities.maxImageExtent.height;
		}
		return swap_chain_extent;
	}

	// Most of the cases we define size of the swap_chain images equal to current window's size
	return dp.surfaceCapabilities.currentExtent;
}

VkImageUsageFlags GetSwapChainUsageFlags()
{
	auto& dp = GetDeviceProperties();

	// Color attachment flag must always be supported
	// We can define other usage flags but we always need to check if they are supported
	if (dp.surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
		return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	LOG("VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT image usage is not supported by the swap chain!");
	assert(false);
	return -1;
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
	if (dp.surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		return dp.surfaceCapabilities.currentTransform;
	}
}

VkPresentModeKHR GetSwapChainPresentMode()
{
	auto& dp = GetDeviceProperties();

	// MAILBOX is the lowest latency V-Sync enabled mode (something like triple-buffering) so use it if available
	for (VkPresentModeKHR &presentMode : dp.presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}

	// IMMEDIATE mode allows us to display frames in a V-Sync independent manner so it can introduce screen tearing
	// But this mode is the best for benchmarking purposes if we want to check the real number of FPS
	for (VkPresentModeKHR &presentMode : dp.presentModes) {
		if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			return presentMode;
		}
	}

	// FIFO present mode is always available
	for (VkPresentModeKHR &presentMode : dp.presentModes) {
		if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
			return presentMode;
		}
	}
	LOG("FIFO present mode is not supported by the swap chain!");
	assert(false);
	return static_cast<VkPresentModeKHR>(-1);
}

VKSwapChain::VKSwapChain(VkPhysicalDevice physicalDevice, VkDevice vkDevice, VkSurfaceKHR vkSurface) :
	device(vkDevice)
{
	CheckSurfaceStats(physicalDevice, vkSurface);

	numberOfImages = GetSwapChainNumImages();
	format = GetSwapChainFormat();
	extent = GetSwapChainExtent();
	usage = GetSwapChainUsageFlags();
	transform = GetSwapChainTransform();
	presentMode = GetSwapChainPresentMode();

	// 跟全屏有关，这里我们不使用
	VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;

	if ((extent.width == 0) || (extent.height == 0)) {
		// Current surface size is (0, 0) so we can't create a swap chain and render anything (CanRender == false)
		// But we don't wont to kill the application as this situation may occur i.e. when window gets minimized
		return;
	}

	LOG("numberOfImages: %d\n", numberOfImages);
	LOG("format: %d %d\n", format.format, format.colorSpace);
	LOG("extent: %d %d\n", extent.width, extent.height);
	LOG("usage: %d\n", usage);
	LOG("transform: %d\n", transform);
	LOG("presentMode: %d\n", presentMode);

	if (windowWidth != extent.width || windowHeight != extent.height)
	{
		LOG("windowWidth or windowHeight has been changed.");
		windowWidth = extent.width;
		windowHeight = extent.height;
	}

	VkSwapchainCreateInfoKHR swapchainCI = {};
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = nullptr;
	swapchainCI.flags = 0;
	swapchainCI.surface = vkSurface;
	swapchainCI.minImageCount = numberOfImages;
	swapchainCI.imageFormat = format.format;
	swapchainCI.imageColorSpace = format.colorSpace;
	swapchainCI.imageExtent = extent;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageUsage = usage;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.pQueueFamilyIndices = nullptr;
	swapchainCI.preTransform = transform;
	swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCI.presentMode = presentMode;
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.oldSwapchain = oldSwapChain;

	// 先不管
	//// Enable transfer source on swap chain images if supported
	//if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
	//	swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	//}

	//// Enable transfer destination on swap chain images if supported
	//if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
	//	swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	//}

	VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChain));

	// If an existing swap chain is re-created, destroy the old swap chain
	if (oldSwapChain != VK_NULL_HANDLE)
	{
		for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
			if (swapChainImageViews[i] != VK_NULL_HANDLE) {
				vkDestroyImageView(device, swapChainImageViews[i], nullptr);
				swapChainImageViews[i] = VK_NULL_HANDLE;
			}
		}
		vkDestroySwapchainKHR(device, oldSwapChain, nullptr);
	}

	uint32_t imageCount = 0;
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr));
	swapChainImages.resize(imageCount);
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data()));

	// 创建SwapChain时提供的参数是minImageCount，实际的image数量可能不一样
	// 尤其是使用VK_PRESENT_MODE_MAILBOX_KHR的时候
	if (numberOfImages != imageCount)
	{
		LOG("numberOfImages has been changed.");
		numberOfImages = imageCount;
	}

	swapChainImageViews.resize(imageCount);
	for (uint32_t i = 0; i < imageCount; i++)
	{
		VkImageViewCreateInfo colorAttachmentView = {};
		colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorAttachmentView.pNext = nullptr;
		colorAttachmentView.flags = 0;
		colorAttachmentView.image = swapChainImages[i];
		colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentView.format = format.format;
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

		VK_CHECK_RESULT(vkCreateImageView(device, &colorAttachmentView, nullptr, &swapChainImageViews[i]));
	}
}

VKSwapChain::~VKSwapChain()
{
	if (device == VK_NULL_HANDLE) {
		return;
	}

	for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
		if (swapChainImageViews[i] != VK_NULL_HANDLE) {
			vkDestroyImageView(device, swapChainImageViews[i], nullptr);
			swapChainImageViews[i] = VK_NULL_HANDLE;
		}
	}

	swapChainImages.clear();
	swapChainImageViews.clear();

	if (swapChain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		swapChain = VK_NULL_HANDLE;
	}
}
