#include "VulkanDescriptorPool.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetLayout.h"
#include "Tools.h"

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice * vulkanDevice):
	m_VulkanDevice(vulkanDevice)
{
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_DescriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(m_VulkanDevice->m_LogicalDevice, m_DescriptorPool, nullptr);
		m_DescriptorPool = VK_NULL_HANDLE;
	}
}

void VulkanDescriptorPool::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
	m_DescriptorPoolSizes.push_back({ descriptorType ,count });
}

void VulkanDescriptorPool::Create(uint32_t maxSets)
{
	uint32_t num = static_cast<uint32_t>(m_DescriptorPoolSizes.size());

	std::vector<VkDescriptorPoolSize> descriptorPoolSizes(num);

	for (uint32_t i = 0; i < num; i++) {
		descriptorPoolSizes[i].type = m_DescriptorPoolSizes[i].descriptorType;
		descriptorPoolSizes[i].descriptorCount = m_DescriptorPoolSizes[i].count;
	}

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = nullptr;
	descriptorPoolCreateInfo.flags = 0;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

	VK_CHECK_RESULT(vkCreateDescriptorPool(m_VulkanDevice->m_LogicalDevice, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));
}

VulkanDescriptorSet * VulkanDescriptorPool::AllocateDescriptorSet(VulkanDescriptorSetLayout* vulkanDescriptorSetLayout)
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &vulkanDescriptorSetLayout->m_DescriptorSetLayout;

	VulkanDescriptorSet* vulkanDescriptorSet = new VulkanDescriptorSet(m_VulkanDevice, vulkanDescriptorSetLayout);
	VK_CHECK_RESULT(vkAllocateDescriptorSets(m_VulkanDevice->m_LogicalDevice, &descriptorSetAllocateInfo, &vulkanDescriptorSet->m_DescriptorSet));
	return vulkanDescriptorSet;
}
