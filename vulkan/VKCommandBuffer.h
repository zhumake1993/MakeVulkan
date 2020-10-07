#pragma once

#include "Common.h"
#include "NonCopyable.h"

class VKDevice;
class VKCommandPool;

struct VKCommandBuffer : public NonCopyable
{
	VKCommandBuffer(VKDevice* vkDevice, VKCommandPool* vkCommandPool, VkCommandBufferLevel level);
	~VKCommandBuffer();

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
};