#include "VKContex.h"
#include "GlobalSettings.h"
#include "DeviceProperties.h"
#include "VKTools.h"

bool CheckLayerAvailability(const char *layerName, const std::vector<VkLayerProperties> &availableLayers)
{
	for (size_t i = 0; i < availableLayers.size(); ++i)
	{
		if (strcmp(availableLayers[i].layerName, layerName) == 0)
		{
			return true;
		}
	}
	return false;
}

bool CheckExtensionAvailability(const char *extensionName, const std::vector<VkExtensionProperties> &availableExtensions)
{
	for (size_t i = 0; i < availableExtensions.size(); ++i)
	{
		if (strcmp(availableExtensions[i].extensionName, extensionName) == 0)
		{
			return true;
		}
	}
	return false;
}

void CheckInstanceLayers()
{
	auto& gs = GetGlobalSettings();
	auto& dp = GetDeviceProperties();

	uint32_t layersCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
	dp.availableInstanceLayers.resize(layersCount);
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layersCount, dp.availableInstanceLayers.data()));

	for (size_t i = 0; i < gs.enabledInstanceLayers.size(); ++i)
	{
		if (!CheckLayerAvailability(gs.enabledInstanceLayers[i], dp.availableInstanceLayers))
		{
			LOGE("instance layer %s not support!\n", gs.enabledInstanceLayers[i]);
		}
	}
}

void CheckInstanceExtensions()
{
	auto& gs = GetGlobalSettings();
	auto& dp = GetDeviceProperties();

	uint32_t extensionsCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
	dp.availableInstanceExtensions.resize(extensionsCount);
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, dp.availableInstanceExtensions.data()));

	for (size_t i = 0; i < gs.enabledInstanceExtensions.size(); ++i)
	{
		if (!CheckExtensionAvailability(gs.enabledInstanceExtensions[i], dp.availableInstanceExtensions))
		{
			LOGE("instance extension %s not support!\n", gs.enabledInstanceExtensions[i]);
		}
	}
}

int SelectPhysicalDevice(VkInstance instance)
{
	auto& dp = GetDeviceProperties();

	uint32_t deviceNum = 0;
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &deviceNum, nullptr));
	dp.physicalDevices.resize(deviceNum);
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &deviceNum, dp.physicalDevices.data()));

	// 直接选择第一个物理设备
	return 0;
}

void CheckDeviceFeatures()
{
	auto& gs = GetGlobalSettings();
	auto& dp = GetDeviceProperties();

	int num = sizeof(dp.deviceFeatures) / sizeof(VkBool32);
	auto enable = reinterpret_cast<VkBool32*>(&gs.enabledDeviceFeatures);
	auto available = reinterpret_cast<VkBool32*>(&dp.deviceFeatures);
	for (int i = 0; i < num; i++)
	{
		if (*(enable + i) == VK_TRUE)
		{
			if (*(available + i) != VK_TRUE)
			{
				LOGE("device feature not support: %d", i);
			}
		}
	}
}

void CheckDeviceExtensions(VkPhysicalDevice physicalDevice)
{
	auto& gs = GetGlobalSettings();
	auto& dp = GetDeviceProperties();

	uint32_t extensionsCount = 0;
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr));
	dp.availableDeviceExtensions.resize(extensionsCount);
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, dp.availableDeviceExtensions.data()));

	for (size_t i = 0; i < gs.enabledDeviceExtensions.size(); ++i)
	{
		if (!CheckExtensionAvailability(gs.enabledDeviceExtensions[i], dp.availableDeviceExtensions))
		{
			LOGE("device extension %s not support!\n", gs.enabledDeviceExtensions[i]);
		}
	}
}

int SelectQueueFamilyIndex(VkPhysicalDevice physicalDevice)
{
	auto& dp = GetDeviceProperties();

	uint32_t queueFamiliesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
	dp.queueFamilyProperties.resize(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, dp.queueFamilyProperties.data());

	// 找到一个支持VK_QUEUE_GRAPHICS_BIT操作的QueueFamily
	// VK_QUEUE_COMPUTE_BIT和VK_QUEUE_TRANSFER_BIT先不管
	for (uint32_t i = 0; i < static_cast<uint32_t>(dp.queueFamilyProperties.size()); i++)
	{
		if ((dp.queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			//&& (dp.queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) 
			//&& (dp.queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			)
		{
			return i;
		}
	}

	LOGE("Could not find a matching queue family index");
}

vk::VKContex::VKContex()
{
	// Instance

	auto& gs = GetGlobalSettings();
	auto& dp = GetDeviceProperties();

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = gs.applicationName.c_str();
	appInfo.applicationVersion = gs.applicationVersion;
	appInfo.pEngineName = gs.engineName.c_str();
	appInfo.engineVersion = gs.engineVersion;
	appInfo.apiVersion = gs.apiVersion;

	CheckInstanceLayers();
	CheckInstanceExtensions();

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(gs.enabledInstanceLayers.size());
	instanceCreateInfo.ppEnabledLayerNames = gs.enabledInstanceLayers.data();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(gs.enabledInstanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = gs.enabledInstanceExtensions.data();

	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

	// Device

	selectedPhysicalDeviceIndex = SelectPhysicalDevice(instance);
	physicalDevice = dp.physicalDevices[selectedPhysicalDeviceIndex];

	vkGetPhysicalDeviceProperties(physicalDevice, &dp.deviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &dp.deviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &dp.deviceMemoryProperties);

	CheckDeviceFeatures();
	CheckDeviceExtensions(physicalDevice);

	selectedQueueFamilyIndex = SelectQueueFamilyIndex(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::vector<float> queuePriorities = { 1.0f };

	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = NULL;
	queueInfo.flags = 0;
	queueInfo.queueFamilyIndex = selectedQueueFamilyIndex;
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
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(gs.enabledDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = gs.enabledDeviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &gs.enabledDeviceFeatures;

	VK_CHECK_RESULT(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

	vkGetDeviceQueue(device, selectedQueueFamilyIndex, 0, &queue);
}

vk::VKContex::~VKContex()
{
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void vk::VKContex::Print()
{
	LOG("[VKContex]\n");

	LOG("selected physical device index: %d\n", selectedPhysicalDeviceIndex);
	LOG("selected queue family index: %d\n", selectedQueueFamilyIndex);

	LOG("\n");
}
