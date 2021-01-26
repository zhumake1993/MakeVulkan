#pragma once

#include "Env.h"
#include "NonCopyable.h"

struct VKRenderPass : public NonCopyable
{
	VKRenderPass(VkDevice vkDevice, VkFormat colorFormat, VkFormat depthFormat);
	~VKRenderPass();

	VkRenderPass renderPass = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};