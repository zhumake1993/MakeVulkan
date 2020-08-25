#pragma once

#include "Common.h"

class VulkanDevice;

class VulkanRenderPass
{

public:

	VulkanRenderPass(VulkanDevice* vulkanDevice, VkFormat format);
	~VulkanRenderPass();

	void CleanUp();

private:

	//

public:

	VkRenderPass m_RenderPass = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};