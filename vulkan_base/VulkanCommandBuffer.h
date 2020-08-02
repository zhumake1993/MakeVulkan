#pragma once

#include "VulkanCommon.h"

class VulkanDevice;
class VulkanCommandPool;

class VulkanCommandBuffer
{

public:

	VulkanCommandBuffer(VulkanDevice* vulkanDevice, VulkanCommandPool* vulkanCommandPool, VkCommandBufferLevel level);
	~VulkanCommandBuffer();

	void CleanUp();

private:

	//

public:

	VkCommandBufferLevel m_Level;
	VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
	VulkanCommandPool* m_VulkanCommandPool = nullptr;
};