#pragma once

#include "Common.h"
#include "VulkanDescriptorTypes.h"

class VulkanDevice;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;

class VulkanDescriptorPool
{

public:

	VulkanDescriptorPool(VulkanDevice* vulkanDevice, uint32_t maxSets, DPSizes& sizes);
	~VulkanDescriptorPool();

	VulkanDescriptorSet* AllocateDescriptorSet(VulkanDescriptorSetLayout* vulkanDescriptorSetLayout);

private:

	//

public:

	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

private:

	uint32_t m_MaxSets = 0;
	DPSizes m_Sizes;
	VulkanDevice* m_VulkanDevice = nullptr;
};