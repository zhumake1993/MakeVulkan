#pragma once

#include "Env.h"
#include "Log.h"
#include "Tools.h"

#define VK_CHECK_RESULT(func)																						\
{																													\
	VkResult res = (func);																							\
	if (res != VK_SUCCESS)																							\
	{																												\
		LOG("Fatal : VkResult is \" %s \" in %s at line %d", VulkanErrorToString(res).c_str(), __FILE__, __LINE__); \
		assert(res == VK_SUCCESS);																					\
	}																												\
}

std::string VulkanErrorToString(VkResult errorCode);

bool CheckExtensionAvailability(const char *extensionName, const std::vector<VkExtensionProperties> &availableExtensions);

bool CheckLayerAvailability(const char *layerName, const std::vector<VkLayerProperties> &availableLayers);

std::string PhysicalDeviceTypeString(VkPhysicalDeviceType type);