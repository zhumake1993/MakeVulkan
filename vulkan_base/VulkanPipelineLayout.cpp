#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice * vulkanDevice):
	m_VulkanDevice(vulkanDevice)
{
	// Create the pipeline layout that is used to generate the rendering pipelines that are based on this descriptor set layout
	// In a more complex scenario you would have different pipeline layouts for different descriptor set layouts that could be reused
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	VK_CHECK_RESULT(vkCreatePipelineLayout(m_VulkanDevice->m_LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_PipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(m_VulkanDevice->m_LogicalDevice, m_PipelineLayout, nullptr);
		m_PipelineLayout = VK_NULL_HANDLE;
	}
}
