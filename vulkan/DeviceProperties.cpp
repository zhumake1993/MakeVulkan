#include "DeviceProperties.h"
#include "Tools.h"

DeviceProperties* deviceProperties;

DeviceProperties & GetDeviceProperties()
{
	return *deviceProperties;
}

DeviceProperties::DeviceProperties()
{
	memset(this, 0, sizeof(DeviceProperties));

	// Instance
	applicationName = "Application Name";
	applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	engineName = "Engine Name";
	engineVersion = VK_MAKE_VERSION(1, 0, 0);
	apiVersion = VK_API_VERSION_1_0;

	// Device
	selectedPhysicalDeviceIndex = 0; // 默认选择第一个物理设备

	// SwapChain
}

void DeviceProperties::Log()
{
	// Instance
	for (size_t i = 0; i < availableInstanceLayers.size(); i++) {
		LOG(" %s", availableInstanceLayers[i].layerName);
	}
	LOG("\n");

	LOG("enabled instance layer ( %d ):", static_cast<int>(enabledInstanceLayers.size()));
	for (size_t i = 0; i < enabledInstanceLayers.size(); i++) {
		LOG(" %s", enabledInstanceLayers[i]);
	}
	LOG("\n");

	for (size_t i = 0; i < availableInstanceExtensions.size; i++) {
		LOG(" %s", availableInstanceExtensions[i].extensionName);
	}
	LOG("\n");

	LOG("enabled instance extensions ( %d ):", static_cast<int>(enabledInstanceExtensions.size()));
	for (size_t i = 0; i < enabledInstanceExtensions.size(); i++) {
		LOG(" %s", enabledInstanceExtensions[i]);
	}
	LOG("\n");

	// Device
	for (size_t i = 0; i < availableDeviceExtensions.size(); i++) {
		LOG(" %s(%d)", availableDeviceExtensions[i].extensionName, availableDeviceExtensions[i].specVersion);
	}
	LOG("\n");

	LOG("enabled device extensions ( %d ):", static_cast<int>(enabledDeviceExtensions.size()));
	for (size_t i = 0; i < enabledDeviceExtensions.size(); i++) {
		LOG(" %s", enabledDeviceExtensions[i]);
	}
	LOG("\n");

	// SwapChain
	for (size_t i = 0; i < surfaceFormats.size(); i++) {
		LOG(" %d %d", surfaceFormats[i].format, surfaceFormats[i].colorSpace);
	}
	LOG("\n");

	for (size_t i = 0; i < presentModes.size(); i++) {
		LOG(" %d", presentModes[i]);
	}
	LOG("\n");
}
