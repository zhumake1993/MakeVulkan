#pragma once

#include "Common.h"
#include "NonCopyable.h"

class VKDevice;

struct VKCommandPool : public NonCopyable
{
	VKCommandPool(VKDevice* vkDevice, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex);
	~VKCommandPool();

	VkCommandPool commandPool = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};