#pragma once

#include "Common.h"

class VulkanDevice;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;

class VulkanDescriptorPool
{

	struct DescriptorPoolSize
	{
		VkDescriptorType descriptorType;
		uint32_t count;
	};

public:

	VulkanDescriptorPool(VulkanDevice* vulkanDevice);
	~VulkanDescriptorPool();

	void CleanUp();
	void AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
	void Create(uint32_t maxSets);
	VulkanDescriptorSet* AllocateDescriptorSet(VulkanDescriptorSetLayout* vulkanDescriptorSetLayout);

private:

	//

public:

	uint32_t m_MaxSets = 0;
	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

private:

	std::vector<DescriptorPoolSize> m_DescriptorPoolSizes;
	VulkanDevice* m_VulkanDevice = nullptr;
};