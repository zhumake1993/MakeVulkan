#pragma once

#include "Common.h"

class VulkanDevice;
class VulkanRenderPass;

class VulkanFramebuffer
{

public:

	VulkanFramebuffer(VulkanDevice* vulkanDevice, VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);
	~VulkanFramebuffer();

private:

	//

public:

	VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};