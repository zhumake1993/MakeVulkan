#pragma once

#include "VulkanCommon.h"

class VulkanInstance;
class VulkanSurface;
struct DescriptorSetUpdater;

class VulkanDevice
{

public:

	VulkanDevice(VulkanInstance* vulkanInstance, VulkanSurface* vulkanSurface);
	~VulkanDevice();

	void CleanUp();
	void WaitIdle();
	uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);
	void UpdateDescriptorSets(std::vector<DescriptorSetUpdater*>& descriptorSetUpdaters);

private:

	void ConfigExtensions();
	void ConfigDeviceFeatures();
	void ConfigQueueFamilies();

public:

	VkPhysicalDevice						m_PhysicalDevice;
	VkDevice								m_LogicalDevice;

	VkPhysicalDeviceProperties				m_DeviceProperties;
	VkPhysicalDeviceFeatures				m_DeviceFeatures;
	VkPhysicalDeviceMemoryProperties		m_DeviceMemoryProperties;

	std::vector<VkExtensionProperties>		m_AvailableDeviceExtensions;

	std::vector<VkQueueFamilyProperties>	m_QueueFamilyProperties;
	uint32_t								m_SelectedQueueFamilyIndex;
	VkQueue									m_Queue;

private:

	VulkanInstance*							m_VulkanInstance = nullptr;
	VulkanSurface*							m_VulkanSurface = nullptr;
};