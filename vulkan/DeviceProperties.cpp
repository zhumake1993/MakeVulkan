#include "DeviceProperties.h"
#include "Log.h"

DeviceProperties gDeviceProperties;

DeviceProperties & GetDeviceProperties()
{
	return gDeviceProperties;
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
	memset(this, 0, sizeof(*this));
}

DeviceProperties::~DeviceProperties()
{
}

void DeviceProperties::Print()
{
	LOG("[DeviceProperties]\n");

	// Instance

	LOG("available instance layers ( %d ):", static_cast<int>(availableInstanceLayers.size()));
	for (size_t i = 0; i < availableInstanceLayers.size(); i++)
	{
		LOG(" %s", availableInstanceLayers[i].layerName);
	}
	LOG("\n");

	LOG("available instance extensions ( %d ):", static_cast<int>(availableInstanceExtensions.size()));
	for (size_t i = 0; i < availableInstanceExtensions.size(); i++) {
		LOG(" %s", availableInstanceExtensions[i].extensionName);
	}
	LOG("\n");

	// Device

	LOG("available physical device num: %d\n", static_cast<int>(physicalDevices.size()));
	LOG("selected device : %s, Type : %s\n", deviceProperties.deviceName, PhysicalDeviceTypeString(deviceProperties.deviceType).c_str());
	
	LOG("VkPhysicalDeviceLimits:");
	LOG("\tminUniformBufferOffsetAlignment : %d\n", static_cast<int>(deviceProperties.limits.minUniformBufferOffsetAlignment));
	LOG("\tminTexelBufferOffsetAlignment : %d\n", static_cast<int>(deviceProperties.limits.minTexelBufferOffsetAlignment));
	LOG("\tminStorageBufferOffsetAlignment : %d\n", static_cast<int>(deviceProperties.limits.minStorageBufferOffsetAlignment));
	LOG("\bufferImageGranularity : %d\n", static_cast<int>(deviceProperties.limits.bufferImageGranularity));
	LOG("\ttimestampPeriod : %f\n", deviceProperties.limits.timestampPeriod);

	LOG("available device extensions ( %d ):", static_cast<int>(availableDeviceExtensions.size()));
	for (size_t i = 0; i < availableDeviceExtensions.size(); i++) {
		LOG(" %s(%d)", availableDeviceExtensions[i].extensionName, availableDeviceExtensions[i].specVersion);
	}
	LOG("\n");

	LOG("available queue families ( %d ):\n", static_cast<int>(queueFamilyProperties.size()));
	for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
		LOG("queueFlags: %d, queueCount: %d, timestampValidBits: %d\n", queueFamilyProperties[i].queueFlags, queueFamilyProperties[i].queueCount, queueFamilyProperties[i].timestampValidBits);
	}
}