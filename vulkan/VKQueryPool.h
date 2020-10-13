#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;

struct VKQueryPool : public NonCopyable
{
	VKQueryPool(VKDevice* vkDevice, VkQueryType queryType, uint32_t queryCount);
	~VKQueryPool();

	VkQueryPool queryPool = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};