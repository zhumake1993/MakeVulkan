#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "Buffer.h"
#include "VKResource.h"

struct VKBuffer : public VKResource
{
	VKBuffer(VkDevice vkDevice, VkDeviceSize vkSize, VkBufferUsageFlags vkUsage, VkMemoryPropertyFlags vkMemoryProperty) :
		device(vkDevice), size(vkSize), usage(vkUsage), memoryProperty(vkMemoryProperty) {}
	virtual ~VKBuffer()
	{
		vkDestroyBuffer(device, buffer, nullptr);
		vkFreeMemory(device, memory, nullptr);
	}

	void Map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Unmap();

	void Update(void * data, VkDeviceSize offset, VkDeviceSize size);

	void Flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

	VkDeviceSize size;
	VkBufferUsageFlags usage;
	VkMemoryPropertyFlags memoryProperty;

	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	void* mappedPointer = nullptr;

	VkDevice device = VK_NULL_HANDLE;
};

class BufferImpl : public Buffer
{

public:

	BufferImpl(VKBuffer* buffer);
	virtual ~BufferImpl();

	VKBuffer* GetBuffer() { return m_Buffer; }
	void SetBuffer(VKBuffer* buffer) { m_Buffer = buffer; }

private:

	VKBuffer* m_Buffer;
};

class GarbageCollector;

class BufferManager : public NonCopyable
{
public:

	BufferManager(VkDevice vkDevice, GarbageCollector* gc);
	virtual ~BufferManager();

	VKBuffer* GetStagingBuffer();

	VKBuffer* CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	VKBuffer* CreateTempBuffer(VkDeviceSize size);

private:

	VKBuffer* CreateBufferInternal(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);

private:

	// Staging Buffer
	const uint32_t m_StagingBufferSize = 10 * 1024 * 1024;
	VKBuffer* m_StagingBuffer;

	GarbageCollector* m_GarbageCollector = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;
};