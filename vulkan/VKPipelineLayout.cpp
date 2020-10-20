#include "VKPipelineLayout.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"

VKPipelineLayout::VKPipelineLayout(VKDevice* vkDevice, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, VkShaderStageFlags pcStage, uint32_t pcSize):
	device(vkDevice->device)
{
	VkPushConstantRange pushConstantRange = {};
	VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
	pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCI.pNext = nullptr;
	pipelineLayoutCI.flags = 0;
	pipelineLayoutCI.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutCI.pSetLayouts = descriptorSetLayouts.data();
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

	VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout));
}

VKPipelineLayout::~VKPipelineLayout()
{
	if (device != VK_NULL_HANDLE && pipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;
	}
}