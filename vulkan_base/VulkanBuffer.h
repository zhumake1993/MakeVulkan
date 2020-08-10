#pragma once

#include "VulkanCommon.h"

class VulkanDevice;

class VulkanBuffer
{

public:

	VulkanBuffer(VulkanDevice* vulkanDevice, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	~VulkanBuffer();

	void CleanUp();
	void MapAndCopy(void * data, uint32_t size);
	void* Map(uint32_t size);
	void Unmap();
	VkDescriptorBufferInfo GetVkDescriptorBufferInfo();

private:

	void CreateBuffer();
	void AllocateMemory();

public:

	uint32_t m_Size;
	VkBufferUsageFlags m_Usage;
	VkMemoryPropertyFlags m_MemoryProperty;

	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};