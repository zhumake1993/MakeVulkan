#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "mkVector.h"
#include "DeviceProperties.h"

namespace vk
{
	struct VKDeviceProperties : public NonCopyable
	{
		VKDeviceProperties();
		~VKDeviceProperties();

		void Init(DeviceProperties& dp);
		void Print();

		// Instance
		mkVector<VkLayerProperties>			availableInstanceLayers;
		mkVector<VkExtensionProperties>		availableInstanceExtensions;

		// Device
		mkVector<VkPhysicalDevice>			physicalDevices;
		VkPhysicalDeviceProperties				deviceProperties;
		VkPhysicalDeviceFeatures				deviceFeatures;
		VkPhysicalDeviceMemoryProperties		deviceMemoryProperties;
		mkVector<VkExtensionProperties>		availableDeviceExtensions;
		mkVector<VkQueueFamilyProperties>	queueFamilyProperties;

		// SwapChain
		VkSurfaceCapabilitiesKHR				surfaceCapabilities;
		mkVector<VkSurfaceFormatKHR>			surfaceFormats;
		mkVector<VkPresentModeKHR>			presentModes;
	};

	VKDeviceProperties& GetVKDeviceProperties();
}