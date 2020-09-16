#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice * vulkanDevice, DSLBindings& bindings):
	m_VulkanDevice(vulkanDevice),
	m_Bindings(bindings)
{
	uint32_t num = static_cast<uint32_t>(bindings.size());

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings(num);

	for (uint32_t i = 0; i < num; i++) {
		layoutBindings[i].binding = i;
		layoutBindings[i].descriptorType = bindings[i].descriptorType;
		layoutBindings[i].descriptorCount = bindings[i].descriptorCount;
		layoutBindings[i].stageFlags = bindings[i].stageFlags;
		layoutBindings[i].pImmutableSamplers = nullptr;
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_VulkanDevice->m_LogicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout));
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_DescriptorSetLayout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(m_VulkanDevice->m_LogicalDevice, m_DescriptorSetLayout, nullptr);
		m_DescriptorSetLayout = VK_NULL_HANDLE;
	}
}

VkDescriptorType VulkanDescriptorSetLayout::GetDescriptorType(uint32_t binding)
{
	return m_Bindings[binding].descriptorType;
}
