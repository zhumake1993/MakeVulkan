#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;
struct VKRenderPass;

struct VKFramebuffer : public NonCopyable
{
	VKFramebuffer(VKDevice* vkDevice, VKRenderPass* vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);
	~VKFramebuffer();

	VkFramebuffer framebuffer = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};