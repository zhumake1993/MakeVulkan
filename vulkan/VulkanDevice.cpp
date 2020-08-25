#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "Tools.h"

VulkanDevice::VulkanDevice(VulkanInstance* vulkanInstance, VulkanSurface* vulkanSurface):
	m_VulkanInstance(vulkanInstance),
	m_VulkanSurface(vulkanSurface)

{
	uint32_t deviceNum = 0;
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(vulkanInstance->m_Instance, &deviceNum, nullptr));
	assert(deviceNum > 0);
	LOG("available physical devices: %d\n", deviceNum);

	std::vector<VkPhysicalDevice> physicalDevices(deviceNum);
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(vulkanInstance->m_Instance, &deviceNum, physicalDevices.data()));
	for (uint32_t i = 0; i < deviceNum; i++) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
		LOG("Device [%d] : %s, Type : %s\n", i, deviceProperties.deviceName, PhysicalDeviceTypeString(deviceProperties.deviceType).c_str());
	}

	// 默认选择第一个物理设备
	m_PhysicalDevice = physicalDevices[0];
	LOG("selected physical device: %d\n", 0);

	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_DeviceProperties);
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_DeviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_DeviceMemoryProperties);

	ConfigDeviceFeatures();

	ConfigExtensions();

	ConfigQueueFamilies();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::vector<float> queuePriorities = { 1.0f };

	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = NULL;
	queueInfo.flags = 0;
	queueInfo.queueFamilyIndex = m_SelectedQueueFamilyIndex;
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
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(global::enabledDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = global::enabledDeviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &global::enabledDeviceFeatures;

	//// If a pNext(Chain) has been passed, we need to add it to the device creation info
	//VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
	//if (pNextChain) {
	//	physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	//	physicalDeviceFeatures2.features = enabledFeatures;
	//	physicalDeviceFeatures2.pNext = pNextChain;
	//	deviceCreateInfo.pEnabledFeatures = nullptr;
	//	deviceCreateInfo.pNext = &physicalDeviceFeatures2;
	//}

	VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice));

	vkGetDeviceQueue(m_LogicalDevice, m_SelectedQueueFamilyIndex, 0, &m_Queue);
}

VulkanDevice::~VulkanDevice()
{
}

void VulkanDevice::CleanUp()
{
	if (m_LogicalDevice != VK_NULL_HANDLE) {
		vkDestroyDevice(m_LogicalDevice, nullptr);
		m_LogicalDevice = VK_NULL_HANDLE;
	}
}

void VulkanDevice::WaitIdle()
{
	if (m_LogicalDevice != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(m_LogicalDevice);
	}
}

uint32_t VulkanDevice::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < m_DeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((m_DeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		typeBits >>= 1;
	}

	LOG("Could not find a matching memory type");
	assert(false);
	return 0;
}

void VulkanDevice::UpdateDescriptorSets(std::vector<DescriptorSetUpdater*>& descriptorSetUpdaters)
{
	uint32_t num = static_cast<uint32_t>(descriptorSetUpdaters.size());

	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.reserve(num);

	for (auto updater : descriptorSetUpdaters) {
		writeDescriptorSets.push_back(updater->Get());
	}

	vkUpdateDescriptorSets(m_LogicalDevice, num, writeDescriptorSets.data(), 0, nullptr);
}

void VulkanDevice::Submit(VulkanCommandBuffer * vulkanCommandBuffer)
{
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkanCommandBuffer->m_CommandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	VK_CHECK_RESULT(vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE));
}

void VulkanDevice::ConfigExtensions()
{
	uint32_t extensionsCount = 0;
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionsCount, nullptr));
	assert(extensionsCount > 0);
	LOG("available device extensions ( %d ):", extensionsCount);

	std::vector<VkExtensionProperties>  availableDeviceExtensions(extensionsCount);
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionsCount, availableDeviceExtensions.data()));
	for (size_t i = 0; i < extensionsCount; i++) {
		LOG(" %s(%d)", availableDeviceExtensions[i].extensionName, availableDeviceExtensions[i].specVersion);
	}
	LOG("\n");

	for (size_t i = 0; i < global::enabledDeviceExtensions.size(); ++i) {
		if (!CheckExtensionAvailability(global::enabledDeviceExtensions[i], availableDeviceExtensions)) {
			LOG("device extension %s not support!\n", global::enabledDeviceExtensions[i]);
			assert(false);
		}
	}

	LOG("enabled device extensions ( %d ):", static_cast<int>(global::enabledDeviceExtensions.size()));
	for (size_t i = 0; i < global::enabledDeviceExtensions.size(); i++) {
		LOG(" %s", global::enabledDeviceExtensions[i]);
	}
	LOG("\n");
}

void VulkanDevice::ConfigDeviceFeatures()
{
	int num = sizeof(m_DeviceFeatures) / sizeof(VkBool32);
	auto enable = reinterpret_cast<VkBool32*>(&global::enabledDeviceFeatures);
	auto available = reinterpret_cast<VkBool32*>(&m_DeviceFeatures);
	for (int i = 0; i < num; i++) {
		if (*(enable + i) == VK_TRUE) {
			if (*(available + i) != VK_TRUE) {
				LOG("device feature not support: %d", i);
				assert(false);
			}
		}
	}
}

void VulkanDevice::ConfigQueueFamilies()
{
	uint32_t queueFamiliesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamiliesCount, nullptr);
	assert(queueFamiliesCount > 0);
	LOG("available queue families ( %d ):\n", queueFamiliesCount);

	m_QueueFamilyProperties.resize(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamiliesCount, m_QueueFamilyProperties.data());
	for (size_t i = 0; i < queueFamiliesCount; i++) {
		LOG("queueFlags: %d, queueCount: %d, timestampValidBits: %d\n", m_QueueFamilyProperties[i].queueFlags, m_QueueFamilyProperties[i].queueCount, m_QueueFamilyProperties[i].timestampValidBits);
	}

	// 找到一个支持所有操作的万能QueueFamily

	m_SelectedQueueFamilyIndex = UINT32_MAX;
	for (uint32_t i = 0; i < static_cast<uint32_t>(m_QueueFamilyProperties.size()); i++)
	{
		if ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && 
			(m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
			(m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
		{
			if (m_VulkanSurface->SupportPresent(m_PhysicalDevice, i)) {
				m_SelectedQueueFamilyIndex = i;
				break;
			}
		}
	}

	if (m_SelectedQueueFamilyIndex == UINT32_MAX) {
		LOG("Could not find a matching queue family index");
	}
}


