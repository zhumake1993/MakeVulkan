#pragma once

#include "Env.h"
#include "Buffer.h"
#include "VKResource.h"

struct VKBuffer : public VKResource
{
	VKBuffer(uint32_t currFrameIndex, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	~VKBuffer();

	void Map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Unmap();

	void Update(void * data, VkDeviceSize offset, VkDeviceSize size);

	void Flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
	void* m_MappedPointer = nullptr;
	VkDevice m_Device = VK_NULL_HANDLE;
};

class VKGarbageCollector;

class VKBufferResource : public Buffer
{

public:

	VKBufferResource(BufferType bufferType, VKGarbageCollector* gc, uint32_t currFrameIndex, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	~VKBufferResource();

	// 一帧内至多调用一次
	void Update(uint32_t currFrameIndex, void * data, VkDeviceSize offset, VkDeviceSize size);

	VKBuffer* GetVKBuffer();

	VkDeviceSize m_Size;
	VkBufferUsageFlags m_Usage;
	VkMemoryPropertyFlags m_MemoryProperty;

private:

	VKBuffer* m_Buffer;
	VKGarbageCollector* m_VKGarbageCollector;
};