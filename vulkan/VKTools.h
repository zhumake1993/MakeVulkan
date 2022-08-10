#pragma once

#include "mkString.h"
#include "VKIncludes.h"
#include "Log.h"

#define VK_CHECK_RESULT(func)																						\
{																													\
	VkResult res = (func);																							\
	if (res != VK_SUCCESS)																							\
	{																												\
		LOG("Fatal : VkResult is \" %s \" in %s at line %d", VkResultToString(res).c_str(), __FILE__, __LINE__); \
		assert(false);																								\
	}																												\
}

mkString VkResultToString(VkResult type);
mkString VkPhysicalDeviceTypeToString(VkPhysicalDeviceType type);
mkString VkFormatToString(VkFormat type);
mkString VkColorSpaceKHRToString(VkColorSpaceKHR type);
mkString VkPresentModeKHRToString(VkPresentModeKHR type);

mkString VkFlagBitToString(VkMemoryHeapFlagBits flagBit);
mkString VkMemoryHeapFlagsToString(VkMemoryHeapFlags flags);

mkString VkFlagBitToString(VkMemoryPropertyFlagBits flagBit);
mkString VkMemoryPropertyFlagsToString(VkMemoryPropertyFlags flags);

mkString VkFlagBitToString(VkQueueFlagBits flagBit);
mkString VkQueueFlagsToString(VkQueueFlags flags);

mkString VkFlagBitToString(VkImageUsageFlagBits flagBit);
mkString VkImageUsageFlagsToString(VkImageUsageFlags flags);

mkString VkFlagBitToString(VkSurfaceTransformFlagBitsKHR flagBit);
mkString VkSurfaceTransformFlagsKHRToString(VkSurfaceTransformFlagsKHR flags);