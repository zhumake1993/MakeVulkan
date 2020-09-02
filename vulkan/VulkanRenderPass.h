#pragma once

#include "Common.h"

class VulkanDevice;

class VulkanRenderPass
{

public:

	VulkanRenderPass(VulkanDevice* vulkanDevice, VkFormat colorFormat, VkFormat depthFormat);
	~VulkanRenderPass();

private:

	//

public:

	VkRenderPass m_RenderPass = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};