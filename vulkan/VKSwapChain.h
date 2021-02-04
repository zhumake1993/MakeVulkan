#pragma once

#include "Env.h"
#include "NonCopyable.h"

struct VKSwapChain : public NonCopyable
{
	VKSwapChain(VkPhysicalDevice physicalDevice, VkDevice vkDevice, VkSurfaceKHR vkSurface);
	virtual ~VKSwapChain();

	uint32_t						numberOfImages;
	VkSurfaceFormatKHR				format;
	VkExtent2D						extent;
	VkImageUsageFlags				usage;
	VkSurfaceTransformFlagBitsKHR	transform;
	VkPresentModeKHR				presentMode;

	VkSwapchainKHR					swapChain = VK_NULL_HANDLE;

	std::vector<VkImage>			swapChainImages;
	std::vector<VkImageView>		swapChainImageViews;

private:

	VkDevice						device = VK_NULL_HANDLE;
};