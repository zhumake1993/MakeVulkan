#pragma once

#include <vector>
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPipeline.h"

class VulkanDevice
{

public:

	VulkanDevice(VulkanInstance* vulkanInstance, VulkanSurface* vulkanSurface);
	~VulkanDevice();

	void CleanUp();
	void WaitIdle();

	uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);
	VulkanCommandPool* CreateCommandPool(VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex);
	VulkanCommandBuffer AllocateCommandBuffers(VulkanCommandPool& pool, VkCommandBufferLevel level);
	VkSemaphore CreateSemaphore();
	VkFence CreateFence(VkFenceCreateFlags flags);
	VulkanShaderModule CreateShaderModule(std::string const &filename);

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