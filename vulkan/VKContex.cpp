#include "VKContex.h"
#include "VKGlobalSettings.h"
#include "VKDeviceProperties.h"
#include "VKTools.h"

bool CheckLayerAvailability(const char *layerName, const mkVector<VkLayerProperties> &availableLayers)
{
	for (int i = 0; i < availableLayers.size(); ++i)
	{
		if (strcmp(availableLayers[i].layerName, layerName) == 0)
		{
			return true;
		}
	}
	return false;
}

bool CheckExtensionAvailability(const char *extensionName, const mkVector<VkExtensionProperties> &availableExtensions)
{
	for (int i = 0; i < availableExtensions.size(); ++i)
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
	auto& vgs = vk::GetVKGlobalSettings();
	auto& vdp = vk::GetVKDeviceProperties();

	uint32_t layersCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
	vdp.availableInstanceLayers.resize(layersCount);
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layersCount, vdp.availableInstanceLayers.data()));

	for (int i = 0; i < vgs.enabledInstanceLayers.size(); ++i)
	{
		if (!CheckLayerAvailability(vgs.enabledInstanceLayers[i], vdp.availableInstanceLayers))
		{
			LOGE("instance layer %s not support!\n", vgs.enabledInstanceLayers[i]);
		}
	}
}

void CheckInstanceExtensions()
{
	auto& vgs = vk::GetVKGlobalSettings();
	auto& vdp = vk::GetVKDeviceProperties();

	uint32_t extensionsCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
	vdp.availableInstanceExtensions.resize(extensionsCount);
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, vdp.availableInstanceExtensions.data()));

	for (int i = 0; i < vgs.enabledInstanceExtensions.size(); ++i)
	{
		if (!CheckExtensionAvailability(vgs.enabledInstanceExtensions[i], vdp.availableInstanceExtensions))
		{
			LOGE("instance extension %s not support!\n", vgs.enabledInstanceExtensions[i]);
		}
	}
}

int SelectPhysicalDevice(VkInstance instance)
{
	auto& vdp = vk::GetVKDeviceProperties();

	uint32_t deviceNum = 0;
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &deviceNum, nullptr));
	vdp.physicalDevices.resize(deviceNum);
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &deviceNum, vdp.physicalDevices.data()));

	// 直接选择第一个物理设备
	return 0;
}

void CheckDeviceFeatures()
{
	auto& vgs = vk::GetVKGlobalSettings();
	auto& vdp = vk::GetVKDeviceProperties();

	int num = sizeof(vdp.deviceFeatures) / sizeof(VkBool32);
	auto enable = reinterpret_cast<VkBool32*>(&vgs.enabledDeviceFeatures);
	auto available = reinterpret_cast<VkBool32*>(&vdp.deviceFeatures);
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
	auto& vgs = vk::GetVKGlobalSettings();
	auto& vdp = vk::GetVKDeviceProperties();

	uint32_t extensionsCount = 0;
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr));
	vdp.availableDeviceExtensions.resize(extensionsCount);
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, vdp.availableDeviceExtensions.data()));

	for (int i = 0; i < vgs.enabledDeviceExtensions.size(); ++i)
	{
		if (!CheckExtensionAvailability(vgs.enabledDeviceExtensions[i], vdp.availableDeviceExtensions))
		{
			LOGE("device extension %s not support!\n", vgs.enabledDeviceExtensions[i]);
		}
	}
}

int SelectQueueFamilyIndex(VkPhysicalDevice physicalDevice)
{
	auto& vdp = vk::GetVKDeviceProperties();

	uint32_t queueFamiliesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
	vdp.queueFamilyProperties.resize(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, vdp.queueFamilyProperties.data());

	// 找到一个支持VK_QUEUE_GRAPHICS_BIT操作的QueueFamily
	// VK_QUEUE_COMPUTE_BIT和VK_QUEUE_TRANSFER_BIT先不管
	for (int i = 0; i < vdp.queueFamilyProperties.size(); i++)
	{
		if ((vdp.queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			//&& (dp.queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) 
			//&& (dp.queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			)
		{
			return i;
		}
	}

	LOGE("Could not find a matching queue family index");
	return -1;
}

vk::VKContex::VKContex()
{
	// Instance

	auto& vgs = vk::GetVKGlobalSettings();
	auto& vdp = vk::GetVKDeviceProperties();

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = vgs.applicationName.c_str();
	appInfo.applicationVersion = vgs.applicationVersion;
	appInfo.pEngineName = vgs.engineName.c_str();
	appInfo.engineVersion = vgs.engineVersion;
	appInfo.apiVersion = vgs.apiVersion;

	CheckInstanceLayers();
	CheckInstanceExtensions();

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(vgs.enabledInstanceLayers.size());
	instanceCreateInfo.ppEnabledLayerNames = vgs.enabledInstanceLayers.data();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(vgs.enabledInstanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = vgs.enabledInstanceExtensions.data();

	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

	// Device

	selectedPhysicalDeviceIndex = SelectPhysicalDevice(instance);
	physicalDevice = vdp.physicalDevices[selectedPhysicalDeviceIndex];

	vkGetPhysicalDeviceProperties(physicalDevice, &vdp.deviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &vdp.deviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &vdp.deviceMemoryProperties);

	CheckDeviceFeatures();
	CheckDeviceExtensions(physicalDevice);

	selectedQueueFamilyIndex = SelectQueueFamilyIndex(physicalDevice);

	mkVector<VkDeviceQueueCreateInfo> queueCreateInfos;
	mkVector<float> queuePriorities = { 1.0f };

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
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(vgs.enabledDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = vgs.enabledDeviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &vgs.enabledDeviceFeatures;

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
