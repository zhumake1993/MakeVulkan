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

// �����ļ�·�������ļ�����
// ����android�������ļ������apk�У�������ѹ���ĸ�ʽ�������Ҫ�����⴦��
std::vector<char> GetBinaryFileContents(std::string const &filename);