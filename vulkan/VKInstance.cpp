#include "VKInstance.h"

#include "DeviceProperties.h"
#include "Tools.h"

void ConfigInstanceLayers()
{
	auto& dp = GetDeviceProperties();

	uint32_t layersCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
	// 移动设备上layersCount有可能是0
	//assert(layersCount > 0);
	LOG("available instance layers ( %d ):", layersCount);

	dp.availableInstanceLayers.resize(layersCount);
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layersCount, dp.availableInstanceLayers.data()));

	for (size_t i = 0; i < dp.enabledInstanceLayers.size(); ++i) {
		if (!CheckLayerAvailability(dp.enabledInstanceLayers[i], dp.availableInstanceLayers)) {
			LOG("instance layer %s not support!\n", dp.enabledInstanceLayers[i]);
			assert(false);
		}
	}
}

void ConfigInstanceExtensions()
{
	auto& dp = GetDeviceProperties();

	uint32_t extensionsCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
	assert(extensionsCount > 0);
	LOG("available instance extensions ( %d ):", extensionsCount);

	dp.availableInstanceExtensions.resize(extensionsCount);
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, dp.availableInstanceExtensions.data()));

	for (size_t i = 0; i < dp.enabledInstanceExtensions.size(); ++i) {
		if (!CheckExtensionAvailability(dp.enabledInstanceExtensions[i], dp.availableInstanceExtensions)) {
			LOG("instance extension %s not support!\n", dp.enabledInstanceExtensions[i]);
			assert(false);
		}
	}
}

VKInstance::VKInstance()
{
	auto& dp = GetDeviceProperties();

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = dp.applicationName.c_str();
	appInfo.applicationVersion = dp.applicationVersion;
	appInfo.pEngineName = dp.engineName.c_str();
	appInfo.engineVersion = dp.engineVersion;
	appInfo.apiVersion = dp.apiVersion;

	ConfigInstanceLayers();

	ConfigInstanceExtensions();

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(dp.enabledInstanceLayers.size());
	instanceCreateInfo.ppEnabledLayerNames = dp.enabledInstanceLayers.data();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(dp.enabledInstanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = dp.enabledInstanceExtensions.data();

	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
}

VKInstance::~VKInstance()
{
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;
	}
}