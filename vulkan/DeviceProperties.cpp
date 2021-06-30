#include "DeviceProperties.h"
#include "Log.h"
#include "mkString.h"

DeviceProperties gDeviceProperties;

DeviceProperties & GetDeviceProperties()
{
	return gDeviceProperties;
}

mkString PhysicalDeviceTypeString(VkPhysicalDeviceType type)
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
	
	LOG("VkPhysicalDeviceLimits:\n");
	LOG("\tminUniformBufferOffsetAlignment : %d\n", static_cast<int>(deviceProperties.limits.minUniformBufferOffsetAlignment));
	LOG("\tminTexelBufferOffsetAlignment : %d\n", static_cast<int>(deviceProperties.limits.minTexelBufferOffsetAlignment));
	LOG("\tminStorageBufferOffsetAlignment : %d\n", static_cast<int>(deviceProperties.limits.minStorageBufferOffsetAlignment));
	LOG("\tbufferImageGranularity : %d\n", static_cast<int>(deviceProperties.limits.bufferImageGranularity));
	LOG("\tnonCoherentAtomSize : %d\n", static_cast<int>(deviceProperties.limits.nonCoherentAtomSize));
	LOG("\ttimestampPeriod : %f\n", deviceProperties.limits.timestampPeriod);

	LOG("VkPhysicalDeviceMemoryProperties:\n");
	for (uint32_t i = 0; i < deviceMemoryProperties.memoryHeapCount; i++)
	{
		VkMemoryHeap& memoryHeap = deviceMemoryProperties.memoryHeaps[i];

		mkString memoryHeapFlags;
		if (memoryHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			memoryHeapFlags += " VK_MEMORY_HEAP_DEVICE_LOCAL_BIT";
		if (memoryHeap.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT)
			memoryHeapFlags += " VK_MEMORY_HEAP_MULTI_INSTANCE_BIT";
		if (memoryHeap.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR)
			memoryHeapFlags += " VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR";
		if (memoryHeapFlags == "")
			memoryHeapFlags = " None";

		LOG("\tMemory heap %u, size = %llu(%lluMB), flags =%s\n", i, memoryHeap.size, memoryHeap.size / 1048576, memoryHeapFlags.c_str());

		for (uint32_t j = 0; j < deviceMemoryProperties.memoryTypeCount; j++)
		{
			VkMemoryType& memoryType = deviceMemoryProperties.memoryTypes[j];

			if (memoryType.heapIndex != i)
				continue;

			mkString memoryPropertyFlags;
			if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				memoryPropertyFlags += " VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT";
			if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				memoryPropertyFlags += " VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT";
			if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				memoryPropertyFlags += " VK_MEMORY_PROPERTY_HOST_COHERENT_BIT";
			if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
				memoryPropertyFlags += " VK_MEMORY_PROPERTY_HOST_CACHED_BIT";
			if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
				memoryPropertyFlags += " VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT";
			if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
				memoryPropertyFlags += " VK_MEMORY_PROPERTY_PROTECTED_BIT";
			if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD)
				memoryPropertyFlags += " VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD";
			if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD)
				memoryPropertyFlags += " VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD";
			if (memoryPropertyFlags == "")
				memoryPropertyFlags = " None";

			LOG("\t\tMemory type %u, flags =%s\n", j, memoryPropertyFlags.c_str());
		}
	}

	LOG("available device extensions ( %d ):", static_cast<int>(availableDeviceExtensions.size()));
	for (size_t i = 0; i < availableDeviceExtensions.size(); i++) {
		LOG(" %s(%d)", availableDeviceExtensions[i].extensionName, availableDeviceExtensions[i].specVersion);
	}
	LOG("\n");

	LOG("available queue families ( %d ):\n", static_cast<int>(queueFamilyProperties.size()));
	for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
		LOG("\tqueueFlags: %d, queueCount: %d, timestampValidBits: %d\n", queueFamilyProperties[i].queueFlags, queueFamilyProperties[i].queueCount, queueFamilyProperties[i].timestampValidBits);
	}

	LOG("\n");
}