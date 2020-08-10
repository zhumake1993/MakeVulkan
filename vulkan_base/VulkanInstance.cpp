#include "VulkanInstance.h"
#include "Tools.h"

VulkanInstance::VulkanInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = global::applicationName.c_str();
	appInfo.applicationVersion = global::applicationVersion;
	appInfo.pEngineName = global::engineName.c_str();
	appInfo.engineVersion = global::engineVersion;
	appInfo.apiVersion = global::apiVersion;

	ConfigInstanceLayers();

	ConfigInstanceExtensions();

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(global::enabledInstanceLayers.size());
	instanceCreateInfo.ppEnabledLayerNames = global::enabledInstanceLayers.data();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(global::enabledInstanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = global::enabledInstanceExtensions.data();

	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance));
}

VulkanInstance::~VulkanInstance()
{
}

void VulkanInstance::CleanUp()
{
	if (m_Instance != VK_NULL_HANDLE) {
		vkDestroyInstance(m_Instance, nullptr);
		m_Instance = VK_NULL_HANDLE;
	}
}

void VulkanInstance::ConfigInstanceLayers()
{
	uint32_t layersCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layersCount, nullptr));
	// 移动设备上layersCount有可能是0
	LOG("available instance layers ( %d ):", layersCount);

	std::vector<VkLayerProperties> availableInstanceLayers(layersCount);
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layersCount, availableInstanceLayers.data()));
	for (size_t i = 0; i < layersCount; i++) {
		LOG(" %s", availableInstanceLayers[i].layerName);
	}
	LOG("\n");

	for (size_t i = 0; i < global::enabledInstanceLayers.size(); ++i) {
		if (!CheckLayerAvailability(global::enabledInstanceLayers[i], availableInstanceLayers)) {
			LOG("instance layer %s not support!\n", global::enabledInstanceLayers[i]);
			assert(false);
		}
	}

	LOG("enabled instance layer ( %d ):", static_cast<int>(global::enabledInstanceLayers.size()));
	for (size_t i = 0; i < global::enabledInstanceLayers.size(); i++) {
		LOG(" %s", global::enabledInstanceLayers[i]);
	}
	LOG("\n");
}

void VulkanInstance::ConfigInstanceExtensions()
{
	uint32_t extensionsCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));
	assert(extensionsCount > 0);
	LOG("available instance extensions ( %d ):", extensionsCount);

	std::vector<VkExtensionProperties> availableInstanceExtensions(extensionsCount);
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, availableInstanceExtensions.data()));
	for (size_t i = 0; i < extensionsCount; i++) {
		LOG(" %s", availableInstanceExtensions[i].extensionName);
	}
	LOG("\n");

	for (size_t i = 0; i < global::enabledInstanceExtensions.size(); ++i) {
		if (!CheckExtensionAvailability(global::enabledInstanceExtensions[i], availableInstanceExtensions)) {
			LOG("instance extension %s not support!\n", global::enabledInstanceExtensions[i]);
			assert(false);
		}
	}

	LOG("enabled instance extensions ( %d ):", static_cast<int>(global::enabledInstanceExtensions.size()));
	for (size_t i = 0; i < global::enabledInstanceExtensions.size(); i++) {
		LOG(" %s", global::enabledInstanceExtensions[i]);
	}
	LOG("\n");
}
