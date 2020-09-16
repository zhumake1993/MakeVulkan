#pragma once

#include "Common.h"

class VulkanDevice;

class VulkanBuffer
{

public:

	VulkanBuffer(VulkanDevice* vulkanDevice, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	~VulkanBuffer();

	void Map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Unmap();
	void Copy(void * data, uint32_t offset, uint32_t size);
	void Flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

private:

	void CreateBuffer();
	void AllocateMemory();

public:

	uint32_t m_Size;
	VkBufferUsageFlags m_Usage;
	VkMemoryPropertyFlags m_MemoryProperty;

	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
	void* m_MappedPointer = nullptr;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};