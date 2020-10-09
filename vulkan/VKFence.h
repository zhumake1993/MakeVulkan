#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;

struct VKFence : public NonCopyable
{
	VKFence(VKDevice* vkDevice, bool signaled);
	~VKFence();

	void Wait();
	void Reset();

	VkFence fence = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};