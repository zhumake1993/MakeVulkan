#pragma once

#include "Env.h"
#include "NonCopyable.h"

struct VKSurface : public NonCopyable
{
	VKSurface(VkInstance vkInstance);
	virtual ~VKSurface();

	VkSurfaceKHR surface = VK_NULL_HANDLE;

private:

	VkInstance instance = VK_NULL_HANDLE;
};