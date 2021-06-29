#pragma once

#include <vector>
#include "VKIncludes.h"
#include "NonCopyable.h"

struct DeviceProperties : public NonCopyable
{
	DeviceProperties();
	~DeviceProperties();

	void Print();

	// Instance
	std::vector<VkLayerProperties>			availableInstanceLayers;
	std::vector<VkExtensionProperties>		availableInstanceExtensions;

	// Device
	std::vector<VkPhysicalDevice>			physicalDevices;
	VkPhysicalDeviceProperties				deviceProperties;
	VkPhysicalDeviceFeatures				deviceFeatures;
	VkPhysicalDeviceMemoryProperties		deviceMemoryProperties;
	std::vector<VkExtensionProperties>		availableDeviceExtensions;
	std::vector<VkQueueFamilyProperties>	queueFamilyProperties;

	// SwapChain
	VkSurfaceCapabilitiesKHR				surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR>			surfaceFormats;
	std::vector<VkPresentModeKHR>			presentModes;
};

DeviceProperties& GetDeviceProperties();