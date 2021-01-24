#pragma once

#include "Env.h"

#define RELEASE(pointer)		\
{								\
	if (pointer != nullptr) {	\
		delete pointer;			\
		pointer = nullptr;		\
	}							\
}

#define EXIT assert(false)

////
//std::string VulkanErrorToString(VkResult errorCode);
//
////
//bool CheckExtensionAvailability(const char *extensionName, const std::vector<VkExtensionProperties> &availableExtensions);
//
////
//bool CheckLayerAvailability(const char *layerName, const std::vector<VkLayerProperties> &availableLayers);

// 根据文件路径返回文件内容
// 由于android上数据文件存放在apk中，并且是压缩的格式，因此需要做特殊处理
std::vector<char> GetBinaryFileContents(std::string const &filename);