#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;

struct VKImage : public NonCopyable
{
	VKImage(VKDevice* vkDevice);
	~VKImage();

	void CreateVkImage();
	void CreateVkImageView();

	// Image
	VkImageCreateFlags         imageCreateFlags = 0;
	VkImageType                imageType = VK_IMAGE_TYPE_2D;
	VkFormat                   format = VK_FORMAT_R8G8B8A8_UNORM;
	uint32_t				   width = 0;
	uint32_t				   height = 0;
	uint32_t				   depth = 1;
	uint32_t                   mipLevels = 1;
	uint32_t                   arrayLayers = 1;
	VkSampleCountFlagBits      samples = VK_SAMPLE_COUNT_1_BIT;
	VkImageTiling              tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageUsageFlags          usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	VkSharingMode              sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	uint32_t                   queueFamilyIndexCount = 0;
	const uint32_t*            pQueueFamilyIndices = nullptr;
	VkImageLayout			   initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	// View
	VkImageViewCreateFlags     imageViewCreateFlags = 0;
	VkImageViewType            viewType = VK_IMAGE_VIEW_TYPE_2D;
	VkComponentSwizzle		   rSwizzle = VK_COMPONENT_SWIZZLE_IDENTITY;
	VkComponentSwizzle		   gSwizzle = VK_COMPONENT_SWIZZLE_IDENTITY;
	VkComponentSwizzle		   bSwizzle = VK_COMPONENT_SWIZZLE_IDENTITY;
	VkComponentSwizzle		   aSwizzle = VK_COMPONENT_SWIZZLE_IDENTITY;
	VkImageAspectFlags		   aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	uint32_t				   baseMipLevel = 0;
	uint32_t				   levelCount = 1;
	uint32_t				   baseArrayLayer = 0;
	uint32_t				   layerCount = 1;

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};