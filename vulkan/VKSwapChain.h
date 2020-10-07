#pragma once

#include "Common.h"
#include "NonCopyable.h"

class VKDevice;
class VKSurface;

struct VKSwapChain : public NonCopyable
{
	VKSwapChain(VKDevice* vkDevice, VKSurface* vkSurface);
	~VKSwapChain();

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