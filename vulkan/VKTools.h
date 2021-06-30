#pragma once

#include "mkString.h"
#include "VKIncludes.h"
#include "Log.h"

#define VK_CHECK_RESULT(func)																						\
{																													\
	VkResult res = (func);																							\
	if (res != VK_SUCCESS)																							\
	{																												\
		LOG("Fatal : VkResult is \" %s \" in %s at line %d", VulkanErrorToString(res).c_str(), __FILE__, __LINE__); \
		assert(false);																								\
	}																												\
}

mkString VulkanErrorToString(VkResult errorCode);