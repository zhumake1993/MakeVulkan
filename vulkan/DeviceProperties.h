#pragma once

#include "mkVector.h"
#include "VKIncludes.h"
#include "NonCopyable.h"

struct DeviceProperties : public NonCopyable
{
	DeviceProperties();
	~DeviceProperties();

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

DeviceProperties& GetDeviceProperties();