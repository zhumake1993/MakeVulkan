#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "Buffer.h"
#include "VKResource.h"

struct VKBufferResource : public VKResource
{
	VKBufferResource(uint32_t currFrameIndex) :VKResource(currFrameIndex) {}
	virtual ~VKBufferResource() {}

	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	void* mappedPointer = nullptr;
};

class VKBuffer : public Buffer
{

public:

	VKBuffer(BufferType bufferType, uint32_t currFrameIndex, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty, bool persistent);
	virtual ~VKBuffer();

	bool InUse(uint32_t currFrameIndex);

	void Map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Unmap();

	void Update(void * data, VkDeviceSize offset, VkDeviceSize size);

	void Flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

	void SwitchResource(VKBuffer* other);

	VkDeviceSize GetSize() { return m_Size; }
	VkBufferUsageFlags GetUsage() { return m_Usage; }
	VkMemoryPropertyFlags GetMemoryProperty() { return m_MemoryProperty; }
	bool GetPersistent() { return m_Persistent; }

	VkBuffer GetBuffer() { return m_BufferResource->buffer; }

private:

	VkDeviceSize m_Size;
	VkBufferUsageFlags m_Usage;
	VkMemoryPropertyFlags m_MemoryProperty;

	bool m_Persistent;

	VKBufferResource* m_BufferResource;
	VkDevice m_Device = VK_NULL_HANDLE;
};

struct VKCommandBuffer;

class BufferManager : public NonCopyable
{
public:

	BufferManager(VkDevice vkDevice);
	virtual ~BufferManager();

	void Update();

	VKBuffer* GetStagingBuffer();

	VKBuffer* CreateBuffer(BufferType bufferType, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty, bool persistent);

	void UpdateBuffer(VKBuffer* buffer, void * data, uint64_t size, VKCommandBuffer* commandBuffer);

	void ReleaseBuffer();

private:

	// Staging Buffer
	const uint32_t m_StagingBufferSize = 10 * 1024 * 1024;
	VKBuffer* m_StagingBuffer;

	// 这一帧新加的Buffer
	std::list<VKBuffer*> m_NewBuffers;

	// 可能还在使用中的Buffer
	std::list<VKBuffer*> m_PendingBuffers;

	uint32_t m_FrameIndex = 0;

	VkDevice m_Device = VK_NULL_HANDLE;
};