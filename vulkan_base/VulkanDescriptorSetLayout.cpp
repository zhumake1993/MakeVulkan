#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice * vulkanDevice):
	m_VulkanDevice(vulkanDevice)
{
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
}

void VulkanDescriptorSetLayout::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_DescriptorSetLayout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(m_VulkanDevice->m_LogicalDevice, m_DescriptorSetLayout, nullptr);
		m_DescriptorSetLayout = VK_NULL_HANDLE;
	}
}

void VulkanDescriptorSetLayout::AddBinding(VkDescriptorType descriptorType, uint32_t count, VkShaderStageFlags shaderStageFlags)
{
	m_DescriptorSetLayoutBindings.push_back({ descriptorType ,count,shaderStageFlags });
}

void VulkanDescriptorSetLayout::Create()
{
	uint32_t num = static_cast<uint32_t>(m_DescriptorSetLayoutBindings.size());

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings(num);

	for (uint32_t i = 0; i < num; i++) {
		layoutBindings[i].binding = i;
		layoutBindings[i].descriptorType = m_DescriptorSetLayoutBindings[i].descriptorType;
		layoutBindings[i].descriptorCount = m_DescriptorSetLayoutBindings[i].descriptorCount;
		layoutBindings[i].stageFlags = m_DescriptorSetLayoutBindings[i].stageFlags;
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

VkDescriptorType VulkanDescriptorSetLayout::GetDescriptorType(uint32_t binding)
{
	return m_DescriptorSetLayoutBindings[binding].descriptorType;
}
