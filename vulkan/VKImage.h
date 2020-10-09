#pragma once

#include "Common.h"
#include "NonCopyable.h"

#define VKImageCI(ci) \
VkImageCreateInfo ci = {}; \
{ \
	ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO; \
	ci.pNext = nullptr; \
	ci.flags = 0; \
	ci.imageType = VK_IMAGE_TYPE_2D; \
	ci.format = VK_FORMAT_R8G8B8A8_UNORM; \
	ci.extent.width = 0; \
	ci.extent.height = 0; \
	ci.extent.depth = 1; \
	ci.mipLevels = 1; \
	ci.arrayLayers = 1; \
	ci.samples = VK_SAMPLE_COUNT_1_BIT; \
	ci.tiling = VK_IMAGE_TILING_OPTIMAL; \
	ci.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; \
	ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE; \
	ci.queueFamilyIndexCount = 0; \
	ci.pQueueFamilyIndices = nullptr; \
	ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; \
}

#define VKImageViewCI(ci) \
VkImageViewCreateInfo ci = {}; \
{ \
	ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; \
	ci.pNext = nullptr; \
	ci.flags = 0; \
	ci.image = VK_NULL_HANDLE; \
	ci.viewType = VK_IMAGE_VIEW_TYPE_2D; \
	ci.format = VK_FORMAT_UNDEFINED; \
	ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; \
	ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY; \
	ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY; \
	ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY; \
	ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; \
	ci.subresourceRange.baseMipLevel = 0; \
	ci.subresourceRange.levelCount = 1; \
	ci.subresourceRange.baseArrayLayer = 0; \
	ci.subresourceRange.layerCount = 1; \
}

struct VKDevice;

struct VKImage : public NonCopyable
{
	VKImage(VKDevice* vkDevice, VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI);
	~VKImage();

	uint32_t GetWidth();
	uint32_t GetHeight();

	VkImage image = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;

private:

	uint32_t width;
	uint32_t height;

	VkDeviceMemory memory = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;
};