#pragma once

#include "Common.h"

class VulkanDevice;

class VulkanImage
{

public:

	VulkanImage(VulkanDevice* vulkanDevice, VkImageType imageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage, VkImageAspectFlags aspect);
	~VulkanImage();

private:

	void CreateImage();
	void AllocateMemory();
	void CreateImageView();
	void CreateSampler();

public:

	VkImageType m_ImageType;
	VkFormat m_Format;
	uint32_t m_Width;
	uint32_t m_Height;
	VkImageUsageFlags m_Usage;
	VkImageAspectFlags m_Aspect;

	VkImage m_Image = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
	VkImageView m_ImageView = VK_NULL_HANDLE;
	VkSampler m_Sampler = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};