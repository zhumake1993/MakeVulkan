#pragma once

#include "Common.h"
#include "NonCopyable.h"

#define VKSamplerCI(ci) \
VkSamplerCreateInfo ci = {}; \
{ \
	ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO; \
	ci.pNext = nullptr; \
	ci.flags = 0; \
	ci.magFilter = VK_FILTER_LINEAR; \
	ci.minFilter = VK_FILTER_LINEAR; \
	ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; \
	ci.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; \
	ci.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; \
	ci.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; \
	ci.mipLodBias = 0.0f; \
	ci.anisotropyEnable = VK_FALSE; \
	ci.maxAnisotropy = 1.0f; \
	ci.compareEnable = VK_FALSE; \
	ci.compareOp = VK_COMPARE_OP_ALWAYS; \
	ci.minLod = 0.0f; \
	ci.maxLod = 0.0f; \
	ci.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK; \
	ci.unnormalizedCoordinates = VK_FALSE; \
}

struct VKDevice;

struct VKSampler : public NonCopyable
{
	VKSampler(VKDevice* vkDevice, VkSamplerCreateInfo& ci);
	~VKSampler();

	VkSampler sampler = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};