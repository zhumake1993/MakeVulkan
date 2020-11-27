#pragma once

#include "Common.h"

#if defined(_WIN32)
	#define LOG(...) printf(__VA_ARGS__)
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "MakeVulkan", __VA_ARGS__))
#endif

#define VK_CHECK_RESULT(func)																						\
{																													\
	VkResult res = (func);																							\
	if (res != VK_SUCCESS)																							\
	{																												\
		LOG("Fatal : VkResult is \" %s \" in %s at line %d", VulkanErrorToString(res).c_str(), __FILE__, __LINE__); \
		assert(res == VK_SUCCESS);																					\
	}																												\
}

#define RELEASE(pointer)		\
{								\
	if (pointer != nullptr) {	\
		delete pointer;			\
		pointer = nullptr;		\
	}							\
}

//
std::string VulkanErrorToString(VkResult errorCode);

//
bool CheckExtensionAvailability(const char *extensionName, const std::vector<VkExtensionProperties> &availableExtensions);

//
bool CheckLayerAvailability(const char *layerName, const std::vector<VkLayerProperties> &availableLayers);

// 根据文件路径返回文件内容
// 由于android上数据文件存放在apk中，并且是压缩的格式，因此需要做特殊处理
std::vector<char> GetBinaryFileContents(std::string const &filename);