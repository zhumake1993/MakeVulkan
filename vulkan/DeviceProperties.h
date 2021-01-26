#pragma once

#include "Env.h"
#include "NonCopyable.h"

struct DeviceProperties : public NonCopyable
{
	DeviceProperties();
	~DeviceProperties();

	uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);

	void Log();

	// Instance

	std::string								applicationName;
	uint32_t								applicationVersion;
	std::string								engineName;
	uint32_t								engineVersion;
	uint32_t								apiVersion;
	std::vector<VkLayerProperties>			availableInstanceLayers;
	std::vector<VkExtensionProperties>		availableInstanceExtensions;

	std::vector<const char*>				enabledInstanceLayers;
	std::vector<const char*>				enabledInstanceExtensions;

	// Device

	std::vector<VkPhysicalDevice>			physicalDevices;
	uint32_t								selectedPhysicalDeviceIndex;
	VkPhysicalDeviceProperties				deviceProperties;
	VkPhysicalDeviceFeatures				deviceFeatures;
	VkPhysicalDeviceMemoryProperties		deviceMemoryProperties;
	std::vector<VkExtensionProperties>		availableDeviceExtensions;

	VkPhysicalDeviceFeatures				enabledDeviceFeatures;
	std::vector<const char*>				enabledDeviceExtensions;

	std::vector<VkQueueFamilyProperties>	queueFamilyProperties;
	uint32_t								selectedQueueFamilyIndex;

	// SwapChain

	VkSurfaceCapabilitiesKHR				surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR>			surfaceFormats;
	std::vector<VkPresentModeKHR>			presentModes;
};

void CreateDeviceProperties();
DeviceProperties& GetDeviceProperties();
void ReleaseDeviceProperties();