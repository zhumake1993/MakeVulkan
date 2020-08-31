#include "VulkanSwapChain.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSemaphore.h"
#include "Tools.h"

VulkanSwapChain::VulkanSwapChain(VulkanDevice* vulkanDevice, VulkanSurface* surface):
	m_VulkanDevice(vulkanDevice),
	m_VulkanSurface(surface)
{
	RecreateSwapChain();
}

VulkanSwapChain::~VulkanSwapChain()
{
}

void VulkanSwapChain::CleanUp()
{
	if (!m_VulkanDevice || m_VulkanDevice->m_LogicalDevice == VK_NULL_HANDLE) {
		return;
	}

	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i) {
		if (m_SwapChainImageViews[i] != VK_NULL_HANDLE) {
			vkDestroyImageView(m_VulkanDevice->m_LogicalDevice, m_SwapChainImageViews[i], nullptr);
			m_SwapChainImageViews[i] = VK_NULL_HANDLE;
		}
	}
	m_SwapChainImages.clear();
	m_SwapChainImageViews.clear();

	if (m_SwapChain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_VulkanDevice->m_LogicalDevice, m_SwapChain, nullptr);
		m_SwapChain = VK_NULL_HANDLE;
	}
}

void VulkanSwapChain::AcquireNextImage(VulkanSemaphore* imageAvailableSemaphore)
{
	VkResult result = vkAcquireNextImageKHR(m_VulkanDevice->m_LogicalDevice, m_SwapChain, UINT64_MAX, imageAvailableSemaphore->m_Semaphore, VK_NULL_HANDLE, &m_ImageIndex);

	switch (result) {
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		LOG("recreate swapchain\n");
		RecreateSwapChain();
		break;
	default:
		LOG("Problem occurred during swap chain image acquisition!\n");
		assert(false);
	}
}

VkImageView VulkanSwapChain::GetCurrImageView()
{
	return m_SwapChainImageViews[m_ImageIndex];
}

void VulkanSwapChain::QueuePresent(VulkanSemaphore* finishedRenderingSemaphore)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &finishedRenderingSemaphore->m_Semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_SwapChain;
	presentInfo.pImageIndices = &m_ImageIndex;
	presentInfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(m_VulkanDevice->m_Queue, &presentInfo);

	switch (result) {
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		LOG("recreate swapchain\n");
		RecreateSwapChain();
		break;
	default:
		LOG("Problem occurred during image presentation!\n");
		assert(false);
	}
}

void VulkanSwapChain::RecreateSwapChain()
{
	if (m_VulkanDevice) {
		m_VulkanDevice->WaitIdle();
	}

	CleanUp();

	CheckSurfaceStats();

	m_NumberOfImages = GetSwapChainNumImages();
	m_Format = GetSwapChainFormat();
	m_Extent = GetSwapChainExtent();
	m_Usage = GetSwapChainUsageFlags();
	m_Transform = GetSwapChainTransform();
	m_PresentMode = GetSwapChainPresentMode();
	//VkSwapchainKHR oldSwapChain = m_SwapChain;

	if ((m_Extent.width == 0) || (m_Extent.height == 0)) {
		// Current surface size is (0, 0) so we can't create a swap chain and render anything (CanRender == false)
		// But we don't wont to kill the application as this situation may occur i.e. when window gets minimized
		return;
	}

	LOG("m_NumberOfImages: %d\n", m_NumberOfImages);
	LOG("m_Format: %d %d\n", m_Format.format, m_Format.colorSpace);
	LOG("m_Extent: %d %d\n", m_Extent.width, m_Extent.height);
	LOG("m_Usage: %d\n", m_Usage);
	LOG("m_Transform: %d\n", m_Transform);
	LOG("m_PresentMode: %d\n", m_PresentMode);

	global::windowWidth = m_Extent.width;
	global::windowHeight = m_Extent.height;

	VkSwapchainCreateInfoKHR swapchainCI = {};
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = nullptr;
	swapchainCI.flags = 0;
	swapchainCI.surface = m_VulkanSurface->m_Surface;
	swapchainCI.minImageCount = m_NumberOfImages;
	swapchainCI.imageFormat = m_Format.format;
	swapchainCI.imageColorSpace = m_Format.colorSpace;
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
	//swapchainCI.oldSwapchain = oldSwapChain;

	//// Enable transfer source on swap chain images if supported
	//if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
	//	swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	//}

	//// Enable transfer destination on swap chain images if supported
	//if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
	//	swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	//}

	VK_CHECK_RESULT(vkCreateSwapchainKHR(m_VulkanDevice->m_LogicalDevice, &swapchainCI, nullptr, &m_SwapChain));

	// If an existing swap chain is re-created, destroy the old swap chain
	//if (oldSwapChain != VK_NULL_HANDLE)
	//{
	//	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i) {
	//		if (m_SwapChainImageViews[i] != VK_NULL_HANDLE) {
	//			vkDestroyImageView(**m_VulkanDevice, m_SwapChainImageViews[i], nullptr);
	//			m_SwapChainImageViews[i] = VK_NULL_HANDLE;
	//		}
	//	}
	//	vkDestroySwapchainKHR(**m_VulkanDevice, oldSwapChain, nullptr);
	//}

	uint32_t imageCount = 0;
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(m_VulkanDevice->m_LogicalDevice, m_SwapChain, &imageCount, nullptr));
	assert(imageCount > 0);
	LOG("imageCount: %d\n", imageCount);

	m_SwapChainImages.resize(imageCount);
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(m_VulkanDevice->m_LogicalDevice, m_SwapChain, &imageCount, m_SwapChainImages.data()));

	m_SwapChainImageViews.resize(imageCount);
	for (uint32_t i = 0; i < imageCount; i++)
	{
		VkImageViewCreateInfo colorAttachmentView = {};
		colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorAttachmentView.pNext = nullptr;
		colorAttachmentView.flags = 0;
		colorAttachmentView.image = m_SwapChainImages[i];
		colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentView.format = m_Format.format;
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

		VK_CHECK_RESULT(vkCreateImageView(m_VulkanDevice->m_LogicalDevice, &colorAttachmentView, nullptr, &m_SwapChainImageViews[i]));
	}
}

void VulkanSwapChain::CheckSurfaceStats()
{
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VulkanDevice->m_PhysicalDevice, m_VulkanSurface->m_Surface, &m_SurfaceCapabilities));

	uint32_t formatsCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_VulkanDevice->m_PhysicalDevice, m_VulkanSurface->m_Surface, &formatsCount, nullptr));
	assert(formatsCount > 0);
	LOG("available formats ( %d ):\n", formatsCount);

	m_SurfaceFormats.resize(formatsCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_VulkanDevice->m_PhysicalDevice, m_VulkanSurface->m_Surface, &formatsCount, m_SurfaceFormats.data()));
	for (size_t i = 0; i < formatsCount; i++) {
		LOG(" %d %d", m_SurfaceFormats[i].format, m_SurfaceFormats[i].colorSpace);
	}
	LOG("\n");

	uint32_t presentModesCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(m_VulkanDevice->m_PhysicalDevice, m_VulkanSurface->m_Surface, &presentModesCount, nullptr));
	assert(presentModesCount > 0);
	LOG("available present modes ( %d ): \n", presentModesCount);

	m_PresentModes.resize(presentModesCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(m_VulkanDevice->m_PhysicalDevice, m_VulkanSurface->m_Surface, &presentModesCount, m_PresentModes.data()));
	for (size_t i = 0; i < presentModesCount; i++) {
		LOG(" %d", m_PresentModes[i]);
	}
	LOG("\n");
}

uint32_t VulkanSwapChain::GetSwapChainNumImages()
{
	// Set of images defined in a swap chain may not always be available for application to render to:
	// One may be displayed and one may wait in a queue to be presented
	// If application wants to use more images at the same time it must ask for more images
	uint32_t imageCount = m_SurfaceCapabilities.minImageCount + 2;
	if ((m_SurfaceCapabilities.maxImageCount > 0) &&
		(imageCount > m_SurfaceCapabilities.maxImageCount)) {
		imageCount = m_SurfaceCapabilities.maxImageCount;
	}
	return imageCount;
}

VkSurfaceFormatKHR VulkanSwapChain::GetSwapChainFormat()
{
	// If the list contains only one entry with undefined format
	// it means that there are no preferred surface formats and any can be chosen
	if ((m_SurfaceFormats.size() == 1) &&
		(m_SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
		return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	// Check if list contains most widely used R8 G8 B8 A8 format
	// with nonlinear color space
	for (VkSurfaceFormatKHR &surfaceformat : m_SurfaceFormats) {
		if (surfaceformat.format == VK_FORMAT_R8G8B8A8_UNORM) {
			return surfaceformat;
		}
	}

	// Return the first format from the list
	return m_SurfaceFormats[0];
}

VkExtent2D VulkanSwapChain::GetSwapChainExtent()
{
	// Special value of surface extent is width == height == -1
	// If this is so we define the size by ourselves but it must fit within defined confines
	if (m_SurfaceCapabilities.currentExtent.width == -1) {
		VkExtent2D swap_chain_extent = { global::windowWidth, global::windowHeight };
		if (swap_chain_extent.width < m_SurfaceCapabilities.minImageExtent.width) {
			swap_chain_extent.width = m_SurfaceCapabilities.minImageExtent.width;
		}
		if (swap_chain_extent.height < m_SurfaceCapabilities.minImageExtent.height) {
			swap_chain_extent.height = m_SurfaceCapabilities.minImageExtent.height;
		}
		if (swap_chain_extent.width > m_SurfaceCapabilities.maxImageExtent.width) {
			swap_chain_extent.width = m_SurfaceCapabilities.maxImageExtent.width;
		}
		if (swap_chain_extent.height > m_SurfaceCapabilities.maxImageExtent.height) {
			swap_chain_extent.height = m_SurfaceCapabilities.maxImageExtent.height;
		}
		return swap_chain_extent;
	}

	// Most of the cases we define size of the swap_chain images equal to current window's size
	return m_SurfaceCapabilities.currentExtent;
}

VkImageUsageFlags VulkanSwapChain::GetSwapChainUsageFlags()
{
	// Color attachment flag must always be supported
	// We can define other usage flags but we always need to check if they are supported
	if (m_SurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
		return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	LOG("VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT image usage is not supported by the swap chain!");
	assert(false);
	return -1;
}

VkSurfaceTransformFlagBitsKHR VulkanSwapChain::GetSwapChainTransform()
{
	// Sometimes images must be transformed before they are presented (i.e. due to device's orienation
	// being other than default orientation)
	// If the specified transform is other than current transform, presentation engine will transform image
	// during presentation operation; this operation may hit performance on some platforms
	// Here we don't want any transformations to occur so if the identity transform is supported use it
	// otherwise just use the same transform as current transform
	if (m_SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		return m_SurfaceCapabilities.currentTransform;
	}
}

VkPresentModeKHR VulkanSwapChain::GetSwapChainPresentMode()
{
	// MAILBOX is the lowest latency V-Sync enabled mode (something like triple-buffering) so use it if available
	for (VkPresentModeKHR &presentMode : m_PresentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}

	// IMMEDIATE mode allows us to display frames in a V-Sync independent manner so it can introduce screen tearing
	// But this mode is the best for benchmarking purposes if we want to check the real number of FPS
	for (VkPresentModeKHR &presentMode : m_PresentModes) {
		if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			return presentMode;
		}
	}

	// FIFO present mode is always available
	for (VkPresentModeKHR &presentMode : m_PresentModes) {
		if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
			return presentMode;
		}
	}
	LOG("FIFO present mode is not supported by the swap chain!");
	assert(false);
	return static_cast<VkPresentModeKHR>(-1);
}