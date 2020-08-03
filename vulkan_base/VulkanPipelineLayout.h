#pragma once

#include "VulkanCommon.h"

class VulkanDevice;

class VulkanPipelineLayout
{

public:

	VulkanPipelineLayout(VulkanDevice* vulkanDevice);
	~VulkanPipelineLayout();

private:

	//

public:

	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};