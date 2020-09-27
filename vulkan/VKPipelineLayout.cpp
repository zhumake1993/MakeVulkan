#include "VKPipelineLayout.h"
#include "Tools.h"

VKPipelineLayout::VKPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkShaderStageFlags pcStage, uint32_t pcSize):
	m_Device(device)
{
	VkPushConstantRange pushConstantRange = {};
	VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
	pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCI.pNext = nullptr;
	pipelineLayoutCI.flags = 0;
	pipelineLayoutCI.setLayoutCount = 1;
	pipelineLayoutCI.pSetLayouts = &descriptorSetLayout;
	if (pcSize > 0) {
		pushConstantRange.stageFlags = pcStage;
		pushConstantRange.offset = 0;
		pushConstantRange.size = pcSize;

		pipelineLayoutCI.pushConstantRangeCount = 1;
		pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
	}
	else {
		pipelineLayoutCI.pushConstantRangeCount = 0;
		pipelineLayoutCI.pPushConstantRanges = nullptr;
	}

	VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device, &pipelineLayoutCI, nullptr, &m_PipelineLayout));
}

VKPipelineLayout::~VKPipelineLayout()
{
	if (m_Device != VK_NULL_HANDLE && m_PipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
		m_PipelineLayout = VK_NULL_HANDLE;
	}
}

VkPipelineLayout VKPipelineLayout::GetLayout()
{
	return m_PipelineLayout;
}
