#pragma once

#include "Common.h"
#include "NonCopyable.h"

class VKInstance;
class VKSurface;

struct VKDevice : public NonCopyable
{
	VKDevice(VKInstance* vkInstance, VKSurface* vkSurface);
	~VKDevice();

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue queue = VK_NULL_HANDLE;
};