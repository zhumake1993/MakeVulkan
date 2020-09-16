#pragma once

#include "Common.h"
#include "VulkanDescriptorTypes.h"

class VulkanDevice;
class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;
class VulkanImage;
class VulkanBuffer;

class VulkanDescriptorSet
{

public:

	VulkanDescriptorSet(VulkanDevice* vulkanDevice, VulkanDescriptorPool* vulkanDescriptorPool, VulkanDescriptorSetLayout* vulkanDescriptorSetLayout);
	~VulkanDescriptorSet();

	VkDescriptorType GetDescriptorType(uint32_t binding);

private:

	//

public:

	VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
	VulkanDescriptorPool* m_VulkanDescriptorPool = nullptr;
	VulkanDescriptorSetLayout* m_VulkanDescriptorSetLayout = nullptr;
};