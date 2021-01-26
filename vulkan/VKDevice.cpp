#include "VKDevice.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"

VkPhysicalDevice SelectPhysicalDevice(VkInstance instance)
{
	auto& dp = GetDeviceProperties();

	uint32_t deviceNum = 0;
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &deviceNum, nullptr));
	dp.physicalDevices.resize(deviceNum);
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &deviceNum, dp.physicalDevices.data()));
	for (uint32_t i = 0; i < deviceNum; i++) {
		// 直接选择第一个物理设备
	}

	VkPhysicalDevice physicalDevice = dp.physicalDevices[dp.selectedPhysicalDeviceIndex];

	return physicalDevice;
}

void CheckDeviceFeatures()
{
	auto& dp = GetDeviceProperties();

	int num = sizeof(dp.deviceFeatures) / sizeof(VkBool32);
	auto enable = reinterpret_cast<VkBool32*>(&dp.enabledDeviceFeatures);
	auto available = reinterpret_cast<VkBool32*>(&dp.deviceFeatures);
	for (int i = 0; i < num; i++) {
		if (*(enable + i) == VK_TRUE) {
			if (*(available + i) != VK_TRUE) {
				LOG("device feature not support: %d", i);
				assert(false);
			}
		}
	}
}

void ConfigExtensions(VkPhysicalDevice physicalDevice)
{
	auto& dp = GetDeviceProperties();

	uint32_t extensionsCount = 0;
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr));
	dp.availableDeviceExtensions.resize(extensionsCount);
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, dp.availableDeviceExtensions.data()));

	for (size_t i = 0; i < dp.enabledDeviceExtensions.size(); ++i) {
		if (!CheckExtensionAvailability(dp.enabledDeviceExtensions[i], dp.availableDeviceExtensions)) {
			LOG("device extension %s not support!\n", dp.enabledDeviceExtensions[i]);
			assert(false);
		}
	}
}

void ConfigQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	auto& dp = GetDeviceProperties();

	uint32_t queueFamiliesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
	dp.queueFamilyProperties.resize(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, dp.queueFamilyProperties.data());

	// 找到一个支持VK_QUEUE_GRAPHICS_BIT操作的QueueFamily
	// VK_QUEUE_COMPUTE_BIT和VK_QUEUE_TRANSFER_BIT先不管

	dp.selectedQueueFamilyIndex = UINT32_MAX;
	for (uint32_t i = 0; i < static_cast<uint32_t>(dp.queueFamilyProperties.size()); i++)
	{
		if ((dp.queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
			//&& (dp.queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) 
			//&& (dp.queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			)
		{
			VkBool32 supportPresent;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportPresent);

			if (supportPresent) {
				dp.selectedQueueFamilyIndex = i;
				break;
			}
		}
	}

	if (dp.selectedQueueFamilyIndex == UINT32_MAX) {
		LOG("Could not find a matching queue family index");
		assert(false);
	}
}

VKDevice::VKDevice(VkInstance vkInstance, VkSurfaceKHR vkSurface)
{
	auto& dp = GetDeviceProperties();

	physicalDevice = SelectPhysicalDevice(vkInstance);

	vkGetPhysicalDeviceProperties(physicalDevice, &dp.deviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &dp.deviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &dp.deviceMemoryProperties);

	CheckDeviceFeatures();

	ConfigExtensions(physicalDevice);

	ConfigQueueFamilies(physicalDevice, vkSurface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::vector<float> queuePriorities = { 1.0f };

	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = NULL;
	queueInfo.flags = 0;
	queueInfo.queueFamilyIndex = dp.selectedQueueFamilyIndex;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = queuePriorities.data();
	queueCreateInfos.push_back(queueInfo);

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(dp.enabledDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = dp.enabledDeviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &dp.enabledDeviceFeatures;

	// 拓展功能，先不管
	//// If a pNext(Chain) has been passed, we need to add it to the device creation info
	//VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
	//if (pNextChain) {
	//	physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	//	physicalDeviceFeatures2.features = enabledFeatures;
	//	physicalDeviceFeatures2.pNext = pNextChain;
	//	deviceCreateInfo.pEnabledFeatures = nullptr;
	//	deviceCreateInfo.pNext = &physicalDeviceFeatures2;
	//}

	VK_CHECK_RESULT(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

	vkGetDeviceQueue(device, dp.selectedQueueFamilyIndex, 0, &queue);
}

VKDevice::~VKDevice()
{
	if (device != VK_NULL_HANDLE) {
		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;
	}
}