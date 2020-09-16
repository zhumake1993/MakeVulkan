#include "VulkanDescriptorSet.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "Tools.h"

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice * vulkanDevice, VulkanDescriptorPool* vulkanDescriptorPool, VulkanDescriptorSetLayout* vulkanDescriptorSetLayout):
	m_VulkanDevice(vulkanDevice),
	m_VulkanDescriptorPool(vulkanDescriptorPool),
	m_VulkanDescriptorSetLayout(vulkanDescriptorSetLayout)
{
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	// 销毁DescriptorPool会自动销毁其中分配的Set
	/*if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_VulkanDescriptorPool->m_DescriptorPool != VK_NULL_HANDLE && m_DescriptorSet != VK_NULL_HANDLE) {
		vkFreeDescriptorSets(m_VulkanDevice->m_LogicalDevice, m_VulkanDescriptorPool->m_DescriptorPool, 1, &m_DescriptorSet);
		m_DescriptorSet = VK_NULL_HANDLE;
	}*/
}

VkDescriptorType VulkanDescriptorSet::GetDescriptorType(uint32_t binding)
{
	return m_VulkanDescriptorSetLayout->GetDescriptorType(binding);
}