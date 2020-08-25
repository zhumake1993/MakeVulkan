#pragma once

#include "Common.h"

class VulkanDevice;

class VulkanDescriptorSetLayout
{

	struct DescriptorSetLayoutBinding
	{
		VkDescriptorType descriptorType;
		uint32_t descriptorCount;
		VkShaderStageFlags stageFlags;
	};

public:

	VulkanDescriptorSetLayout(VulkanDevice* vulkanDevice);
	~VulkanDescriptorSetLayout();

	void CleanUp();
	void AddBinding(VkDescriptorType descriptorType, uint32_t count, VkShaderStageFlags shaderStageFlags);
	void Create();
	VkDescriptorType GetDescriptorType(uint32_t binding);

private:

	//

public:

	VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

private:

	std::vector<DescriptorSetLayoutBinding> m_DescriptorSetLayoutBindings;
	VulkanDevice* m_VulkanDevice = nullptr;
};