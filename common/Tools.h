#pragma once

#include "Common.h"

// 
#if defined(_WIN32)
	#define LOG(...) printf(__VA_ARGS__)
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "MakeVulkan", __VA_ARGS__))
#endif

//
#define VK_CHECK_RESULT(func)																						\
{																													\
	VkResult res = (func);																							\
	if (res != VK_SUCCESS)																							\
	{																												\
		LOG("Fatal : VkResult is \" %s \" in %s at line %d", VulkanErrorToString(res).c_str(), __FILE__, __LINE__); \
		assert(res == VK_SUCCESS);																					\
	}																												\
}

//
std::string VulkanErrorToString(VkResult errorCode);

//
std::string PhysicalDeviceTypeString(VkPhysicalDeviceType type);

//
bool CheckExtensionAvailability(const char *extensionName, const std::vector<VkExtensionProperties> &availableExtensions);

//
bool CheckLayerAvailability(const char *layerName, const std::vector<VkLayerProperties> &availableLayers);

// ************************************************************ //
// GetBinaryFileContents                                        //
//                                                              //
// Function reading binary contents of a file                   //
// ************************************************************ //
std::vector<char> GetBinaryFileContents(std::string const &filename);

std::vector<char> GetImageData(std::string const &filename, int requestedComponents, uint32_t *width, uint32_t *height, uint32_t *components, uint32_t *dataSize);