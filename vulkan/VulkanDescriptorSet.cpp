#include "VulkanDescriptorSet.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "Tools.h"

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice * vulkanDevice, VulkanDescriptorSetLayout* vulkanDescriptorSetLayout):
	m_VulkanDevice(vulkanDevice),
	m_VulkanDescriptorSetLayout(vulkanDescriptorSetLayout)
{
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	/*if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_DescriptorSetLayout != VK_NULL_HANDLE) {
		vkFreeDescriptorSets(m_VulkanDevice->m_LogicalDevice, m_DescriptorSetLayout, nullptr);
		m_DescriptorSetLayout = VK_NULL_HANDLE;
	}*/
}

VkDescriptorType VulkanDescriptorSet::GetDescriptorType(uint32_t binding)
{
	return m_VulkanDescriptorSetLayout->GetDescriptorType(binding);
}

DescriptorSetUpdater::DescriptorSetUpdater(VulkanDescriptorSet * vulkanDescriptorSet, uint32_t binding, uint32_t index)
{
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = vulkanDescriptorSet->m_DescriptorSet;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = index;
	writeDescriptorSet.descriptorCount = 0;
	writeDescriptorSet.descriptorType = vulkanDescriptorSet->GetDescriptorType(binding);
	writeDescriptorSet.pImageInfo = nullptr;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;
}

void DescriptorSetUpdater::AddImage(VulkanImage * vulkanImage)
{
	writeDescriptorSet.descriptorCount++;
	descriptorImageInfos.push_back(vulkanImage->GetVkDescriptorImageInfo());
}

void DescriptorSetUpdater::AddBuffer(VulkanBuffer * vulkanBuffer)
{
	writeDescriptorSet.descriptorCount++;
	descriptorBufferInfos.push_back(vulkanBuffer->GetVkDescriptorBufferInfo());
}

VkWriteDescriptorSet DescriptorSetUpdater::Get()
{
	if (descriptorImageInfos.size() > 0) {
		writeDescriptorSet.pImageInfo = descriptorImageInfos.data();
	}
	if (descriptorBufferInfos.size() > 0) {
		writeDescriptorSet.pBufferInfo = descriptorBufferInfos.data();
	}
	if (bufferViews.size() > 0) {
		writeDescriptorSet.pTexelBufferView = bufferViews.data();
	}

	return writeDescriptorSet;
}
