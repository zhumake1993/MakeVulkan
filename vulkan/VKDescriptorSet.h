#pragma once

#include "Env.h"
#include "VKResource.h"

union DescriptorInfo
{
	VkDescriptorBufferInfo buffer;
	VkDescriptorImageInfo image;
	VkBufferView texelBufferView;
};

struct VKDescriptorSet : public VKResource
{
	VKDescriptorSet(uint32_t currFrameIndex);
	~VKDescriptorSet();

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};