#pragma once

#include "Common.h"

class VKPipelineLayout
{

public:

	VKPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkShaderStageFlags pcStage, uint32_t pcSize);
	~VKPipelineLayout();

	VkPipelineLayout GetLayout();

private:

	//

public:

	//

private:

	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

	VkDevice m_Device = VK_NULL_HANDLE;
};