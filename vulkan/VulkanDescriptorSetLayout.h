#pragma once

#include "Common.h"
#include "VulkanDescriptorTypes.h"

class VulkanDevice;

class VulkanDescriptorSetLayout
{

public:

	VulkanDescriptorSetLayout(VulkanDevice* vulkanDevice, DSLBindings& bindings);
	~VulkanDescriptorSetLayout();

	VkDescriptorType GetDescriptorType(uint32_t binding);

private:

	//

public:

	VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

private:

	DSLBindings m_Bindings;
	VulkanDevice* m_VulkanDevice = nullptr;
};