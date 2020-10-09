#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;

struct VKPipelineLayout : public NonCopyable
{
	VKPipelineLayout(VKDevice* vkDevice, VkDescriptorSetLayout descriptorSetLayout, VkShaderStageFlags pcStage, uint32_t pcSize);
	~VKPipelineLayout();

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};