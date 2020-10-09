#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;
struct VKRenderPass;

struct VKRenderPass : public NonCopyable
{
	VKRenderPass(VKDevice* vkDevice, VkFormat colorFormat, VkFormat depthFormat);
	~VKRenderPass();

	VkRenderPass renderPass = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};