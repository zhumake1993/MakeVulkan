#pragma once

#include "Env.h"
#include "NonCopyable.h"

struct VKDevice : public NonCopyable
{
	VKDevice(VkInstance vkInstance, VkSurfaceKHR vkSurface);
	~VKDevice();

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue queue = VK_NULL_HANDLE;
};