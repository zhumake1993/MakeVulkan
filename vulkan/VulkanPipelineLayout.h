#pragma once

#include "Common.h"

class VulkanDevice;

class VulkanPipelineLayout
{

public:

	VulkanPipelineLayout(VulkanDevice* vulkanDevice, VkDescriptorSetLayout descriptorSetLayout);
	~VulkanPipelineLayout();

private:

	//

public:

	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};