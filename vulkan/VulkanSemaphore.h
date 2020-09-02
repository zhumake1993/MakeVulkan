#pragma once

#include "Common.h"

class VulkanDevice;

class VulkanSemaphore
{

public:

	VulkanSemaphore(VulkanDevice* vulkanDevice);
	~VulkanSemaphore();

private:

	//

public:

	VkSemaphore m_Semaphore = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};