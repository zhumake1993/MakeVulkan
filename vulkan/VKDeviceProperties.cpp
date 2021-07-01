#include "VKDeviceProperties.h"
#include "Log.h"
#include "VKTools.h"

namespace vk
{
	VKDeviceProperties gVKDeviceProperties;

	VKDeviceProperties & GetVKDeviceProperties()
	{
		return gVKDeviceProperties;
	}

	VKDeviceProperties::VKDeviceProperties()
	{
	}

	VKDeviceProperties::~VKDeviceProperties()
	{
	}

	void VKDeviceProperties::Init(DeviceProperties & dp)
	{
		dp.deviceName = deviceProperties.deviceName;

		dp.minUniformBufferOffsetAlignment = deviceProperties.limits.minUniformBufferOffsetAlignment;
	}

	void VKDeviceProperties::Print()
	{
		LOG("[DeviceProperties]\n");

		// Instance

		LOG("available instance layers ( %d ):", availableInstanceLayers.size());
		for (int i = 0; i < availableInstanceLayers.size(); i++)
		{
			LOG(" %s", availableInstanceLayers[i].layerName);
		}
		LOG("\n");

		LOG("available instance extensions ( %d ):", availableInstanceExtensions.size());
		for (int i = 0; i < availableInstanceExtensions.size(); i++)
		{
			LOG(" %s", availableInstanceExtensions[i].extensionName);
		}
		LOG("\n");

		// Device

		LOG("available physical device num: %d\n", physicalDevices.size());
		LOG("selected device : %s, Type : %s\n", deviceProperties.deviceName, VkPhysicalDeviceTypeToString(deviceProperties.deviceType).c_str());

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

			LOG("\tMemory heap %u, size = %llu(%lluMB), flags = %s\n", i, memoryHeap.size, memoryHeap.size / 1048576, VkMemoryHeapFlagsToString(memoryHeap.flags).c_str());

			for (uint32_t j = 0; j < deviceMemoryProperties.memoryTypeCount; j++)
			{
				VkMemoryType& memoryType = deviceMemoryProperties.memoryTypes[j];

				if (memoryType.heapIndex != i)
					continue;

				LOG("\t\tMemory type %u, flags = %s\n", j, VkMemoryPropertyFlagsToString(memoryType.propertyFlags).c_str());
			}
		}

		LOG("available device extensions ( %d ):", availableDeviceExtensions.size());
		for (int i = 0; i < availableDeviceExtensions.size(); i++)
		{
			LOG(" %s(%d)", availableDeviceExtensions[i].extensionName, availableDeviceExtensions[i].specVersion);
		}
		LOG("\n");

		LOG("available queue families ( %d ):\n", queueFamilyProperties.size());
		for (int i = 0; i < queueFamilyProperties.size(); i++)
		{
			LOG("\tqueueFlags: %s, queueCount: %d, timestampValidBits: %d\n", VkQueueFlagsToString(queueFamilyProperties[i].queueFlags).c_str(), queueFamilyProperties[i].queueCount, queueFamilyProperties[i].timestampValidBits);
		}

		LOG("\n");
	}
}