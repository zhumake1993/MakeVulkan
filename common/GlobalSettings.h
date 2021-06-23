#pragma once

#include <string>
#include <vector>
#include "NonCopyable.h"
#include "VKIncludes.h"

// ʹ��Visual Leak Detector������PC��
// Ϊ�˷��㣬�Ұ�vld�İ���·��ֱ�Ӽӵ�ϵͳ��path��ȥ��
#ifdef _WIN32
#define USE_VISUAL_LEAK_DETECTOR 0
#endif

#define FrameResourcesCount 3

struct GlobalSettings : public NonCopyable
{
	GlobalSettings();
	~GlobalSettings();

	void Print();

#if defined(_WIN32)
	std::string consoleTitle;
	std::string windowClassName;
	std::string windowTitleName;
	uint32_t	windowWidth;
	uint32_t	windowHeight;
#endif

	std::string AssetPath;

	// Instance
	std::string								applicationName;
	uint32_t								applicationVersion;
	std::string								engineName;
	uint32_t								engineVersion;
	uint32_t								apiVersion;
	std::vector<const char*>				enabledInstanceLayers;
	std::vector<const char*>				enabledInstanceExtensions;

	// Device
	VkPhysicalDeviceFeatures				enabledDeviceFeatures;
	std::vector<const char*>				enabledDeviceExtensions;

	// Memory
	VkDeviceSize memoryAlignment = 256; // 256����ͬʱ����minTexelBufferOffsetAlignment, minUniformBufferOffsetAlignment, minStorageBufferOffsetAlignment�Ķ���Ҫ��
	VkDeviceSize memoryBlockSize = 64 * 1024 * 1024;
};

GlobalSettings& GetGlobalSettings();