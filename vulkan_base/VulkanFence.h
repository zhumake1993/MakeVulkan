#pragma once

#include "VulkanCommon.h"

class VulkanDevice;

class VulkanFence
{

public:

	VulkanFence(VulkanDevice* vulkanDevice, bool signaled);
	~VulkanFence();

	void CleanUp();

private:

	//

public:

	bool m_Signaled = false;
	VkFence m_Fence = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};