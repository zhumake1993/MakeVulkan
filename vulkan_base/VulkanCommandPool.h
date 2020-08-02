#pragma once

#include "VulkanCommon.h"

class VulkanDevice;
class VulkanCommandBuffer;

class VulkanCommandPool
{

public:

	VulkanCommandPool(VulkanDevice* vulkanDevice, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex);
	~VulkanCommandPool();

	void CleanUp();
	VulkanCommandBuffer* AllocateCommandBuffer(VkCommandBufferLevel level);

private:

	//

public:

	VkCommandPoolCreateFlags m_Flags;
	uint32_t m_QueueFamilyIndex;
	VkCommandPool m_CommandPool = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;

};