#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;

struct VKSemaphore : public NonCopyable
{
	VKSemaphore(VKDevice* vkDevice);
	~VKSemaphore();

	VkSemaphore semaphore = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};