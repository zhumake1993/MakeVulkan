#pragma once

#include "Common.h"

class VulkanDevice;
class VulkanDescriptorSetLayout;
class VulkanImage;
class VulkanBuffer;

class VulkanDescriptorSet
{

public:

	VulkanDescriptorSet(VulkanDevice* vulkanDevice, VulkanDescriptorSetLayout* vulkanDescriptorSetLayout);
	~VulkanDescriptorSet();

	void CleanUp();
	VkDescriptorType GetDescriptorType(uint32_t binding);

private:

	//

public:

	VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
	VulkanDescriptorSetLayout* m_VulkanDescriptorSetLayout = nullptr;
};

struct DescriptorSetUpdater
{
	DescriptorSetUpdater(VulkanDescriptorSet* vulkanDescriptorSet, uint32_t binding, uint32_t index);

	void AddImage(VulkanImage* vulkanImage);

	void AddBuffer(VulkanBuffer* vulkanBuffer);

	VkWriteDescriptorSet Get();

private:

	VkWriteDescriptorSet writeDescriptorSet;

	std::vector<VkDescriptorImageInfo> descriptorImageInfos;
	std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
	std::vector<VkBufferView> bufferViews;
};