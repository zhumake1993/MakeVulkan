#include "DeviceProperties.h"
#include "Tools.h"

DeviceProperties* gDeviceProperties = nullptr;

DeviceProperties & GetDeviceProperties()
{
	if (!gDeviceProperties) {
		gDeviceProperties = new DeviceProperties();
	}
	return *gDeviceProperties;
}

void ReleaseDeviceProperties()
{
	RELEASE(gDeviceProperties);
}

std::string PhysicalDeviceTypeString(VkPhysicalDeviceType type)
{
	switch (type)
	{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_##r: return #r
		STR(OTHER);
		STR(INTEGRATED_GPU);
		STR(DISCRETE_GPU);
		STR(VIRTUAL_GPU);
#undef STR
	default: return "UNKNOWN_DEVICE_TYPE";
	}
}

DeviceProperties::DeviceProperties()
{
	//memset(this, 0, sizeof(DeviceProperties));

	// Instance
	applicationName = "Application Name";
	applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	engineName = "Engine Name";
	engineVersion = VK_MAKE_VERSION(1, 0, 0);
	apiVersion = VK_API_VERSION_1_0;

	// Device
	selectedPhysicalDeviceIndex = 0; // 默认选择第一个物理设备
	memset(&enabledDeviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));

	// SwapChain
}

DeviceProperties::~DeviceProperties()
{
}

void DeviceProperties::Log()
{
	// Instance

	LOG("\nInstance:\n");

	LOG("available instance layers ( %d ):", static_cast<int>(availableInstanceLayers.size()));
	for (size_t i = 0; i < availableInstanceLayers.size(); i++) {
		LOG(" %s", availableInstanceLayers[i].layerName);
	}
	LOG("\n");

	LOG("enabled instance layer ( %d ):", static_cast<int>(enabledInstanceLayers.size()));
	for (size_t i = 0; i < enabledInstanceLayers.size(); i++) {
		LOG(" %s", enabledInstanceLayers[i]);
	}
	LOG("\n");

	LOG("available instance extensions ( %d ):", static_cast<int>(availableInstanceExtensions.size()));
	for (size_t i = 0; i < availableInstanceExtensions.size(); i++) {
		LOG(" %s", availableInstanceExtensions[i].extensionName);
	}
	LOG("\n");

	LOG("enabled instance extensions ( %d ):", static_cast<int>(enabledInstanceExtensions.size()));
	for (size_t i = 0; i < enabledInstanceExtensions.size(); i++) {
		LOG(" %s", enabledInstanceExtensions[i]);
	}
	LOG("\n");

	// Device

	LOG("\nDevice:\n");

	LOG("available physical devices: %d\n", static_cast<int>(physicalDevices.size()));
	LOG("selected physical device: %d\n", selectedPhysicalDeviceIndex);
	LOG("Device : %s, Type : %s\n", deviceProperties.deviceName, PhysicalDeviceTypeString(deviceProperties.deviceType).c_str());
	LOG("VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment : %d\n", static_cast<int>(deviceProperties.limits.minUniformBufferOffsetAlignment));
	LOG("VkPhysicalDeviceLimits::timestampPeriod : %f\n", deviceProperties.limits.timestampPeriod);

	LOG("available device extensions ( %d ):", static_cast<int>(availableDeviceExtensions.size()));
	for (size_t i = 0; i < availableDeviceExtensions.size(); i++) {
		LOG(" %s(%d)", availableDeviceExtensions[i].extensionName, availableDeviceExtensions[i].specVersion);
	}
	LOG("\n");

	LOG("enabled device extensions ( %d ):", static_cast<int>(enabledDeviceExtensions.size()));
	for (size_t i = 0; i < enabledDeviceExtensions.size(); i++) {
		LOG(" %s", enabledDeviceExtensions[i]);
	}
	LOG("\n");

	LOG("available queue families ( %d ):\n", static_cast<int>(queueFamilyProperties.size()));
	for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
		LOG("queueFlags: %d, queueCount: %d, timestampValidBits: %d\n", queueFamilyProperties[i].queueFlags, queueFamilyProperties[i].queueCount, queueFamilyProperties[i].timestampValidBits);
	}

	LOG("selected queue family: %d\n", selectedQueueFamilyIndex);

	// SwapChain

	LOG("\nSwapChain:\n");

	LOG("surfaceFormats:\n");
	for (size_t i = 0; i < surfaceFormats.size(); i++) {
		LOG(" %d %d", surfaceFormats[i].format, surfaceFormats[i].colorSpace);
	}
	LOG("\n");

	LOG("presentModes:\n");
	for (size_t i = 0; i < presentModes.size(); i++) {
		LOG(" %d", presentModes[i]);
	}
	LOG("\n");
}
