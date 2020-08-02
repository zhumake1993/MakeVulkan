#pragma once

#include "VulkanCommon.h"

class VulkanCommandBuffer
{

public:

	VulkanCommandBuffer();
	~VulkanCommandBuffer();

	VkCommandBuffer& operator*();

private:

	//

public:

	VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
};