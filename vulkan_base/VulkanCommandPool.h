#pragma once

#include "VulkanCommon.h"

class VulkanDevice;

class VulkanCommandPool
{

public:

	VulkanCommandPool(VulkanDevice* vulkanDevice);
	~VulkanCommandPool();

	void CleanUp();

private:

	//

public:
	VkCommandPoolCreateFlags m_Flags;
	uint32_t m_QueueFamilyIndex;
	VkCommandPool m_CommandPool = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;

};