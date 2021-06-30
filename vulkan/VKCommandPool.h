#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"

struct VKCommandPool : public NonCopyable
{
	VKCommandPool(VkDevice vkDevice, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex);
	virtual ~VKCommandPool();

	VkCommandPool commandPool = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};