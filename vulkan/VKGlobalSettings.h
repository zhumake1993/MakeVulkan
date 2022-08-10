#pragma once

#include "mkString.h"
#include "mkVector.h"
#include "NonCopyable.h"
#include "VKIncludes.h"

namespace vk
{
	struct VKGlobalSettings : public NonCopyable
	{
		VKGlobalSettings();
		~VKGlobalSettings();

		void Print();

		// Instance
		mkString applicationName{ "Application Name" };
		uint32_t applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		mkString engineName{ "Engine Name" };
		uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
		uint32_t apiVersion = VK_MAKE_VERSION(1, 0, 0);
		mkVector<const char*> enabledInstanceLayers;
		mkVector<const char*> enabledInstanceExtensions;

		// Device
		VkPhysicalDeviceFeatures enabledDeviceFeatures;
		mkVector<const char*> enabledDeviceExtensions;

		// Memory
		VkDeviceSize memoryAlignment = 256; // 256可以同时满足minTexelBufferOffsetAlignment, minUniformBufferOffsetAlignment, minStorageBufferOffsetAlignment的对齐要求
		VkDeviceSize memoryBlockSize = 8 * 1024 * 1024;
	};

	VKGlobalSettings& GetVKGlobalSettings();
}