#pragma once

#include "Env.h"
#include "Image.h"

class VKImage : public Image
{
public:

	VKImage(ImageType imageType, VkDevice vkDevice, VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage,
		VkImageViewType vkImageViewType, VkImageAspectFlags aspectMask);
	virtual ~VKImage();

	VkImage m_Image = VK_NULL_HANDLE;
	VkImageView m_View = VK_NULL_HANDLE;

private:

	VkDeviceMemory m_Memory = VK_NULL_HANDLE;

	VkDevice m_Device = VK_NULL_HANDLE;
};