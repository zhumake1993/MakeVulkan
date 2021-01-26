#pragma once

#include "Env.h"
#include "Buffer.h"

class VKBuffer : public Buffer
{
public:

	VKBuffer(BufferType bufferType, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	~VKBuffer();

	void Map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Unmap();

	void Update(void * data, VkDeviceSize offset, VkDeviceSize size);

	void Flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

	VkDeviceSize m_Size;
	VkBufferUsageFlags m_Usage;
	VkMemoryPropertyFlags m_MemoryProperty;

	VkBuffer m_Buffer = VK_NULL_HANDLE;

private:

	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
	void* m_MappedPointer = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;
};