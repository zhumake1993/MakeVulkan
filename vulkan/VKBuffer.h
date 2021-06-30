#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "GfxBuffer.h"
#include "VKMemory.h"
#include "GfxTypes.h"

class VKCommandBuffer;

namespace vk
{
	class BufferManager;

	class BufferResource : public VKResource
	{
	public:

		BufferResource(VkDevice device, MemoryAllocator& allocator, VkBuffer buffer, Memory memory);
		virtual ~BufferResource();

		VkBuffer GetBuffer();

		VkBuffer AccessBuffer();

		void Update(void* data, uint64_t offset, uint64_t size, VKCommandBuffer* cmdBuffer = nullptr, uint64_t frameIndex = 0, BufferManager* bufferManager = nullptr);

		void Flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
		void Invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

	private:

		VkMappedMemoryRange MakeMappedMemoryRange(VkDeviceSize offset, VkDeviceSize size);

	private:

		VkDevice m_Device;
		MemoryAllocator& m_Allocator;
		VkBuffer m_Buffer;
		Memory m_Memory;
	};

	class VulkanBuffer : public GfxBuffer
	{

	public:

		VulkanBuffer(BufferManager* bufferManager, GfxBufferUsage bufferUsage, GfxBufferMode bufferMode, uint64_t size);
		virtual ~VulkanBuffer();

		VkBuffer GetBuffer();

		VkBuffer AccessBuffer();

		void Update(void* data, uint64_t offset, uint64_t size, VKCommandBuffer* cmdBuffer, uint64_t frameIndex);

	private:

		BufferResource * CreateBufferResource();

	private:

		BufferManager* m_BufferManager;
		BufferResource* m_BufferResource;
	};

	class BufferManager : NonCopyable
	{
	public:

		BufferManager(VkDevice device, MemoryAllocator& allocator);
		virtual ~BufferManager();

		BufferResource * CreateBufferResource(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags);
		BufferResource * CreateTransientStagingBufferResource(size_t size);
		BufferResource * CreateTransientUniformBufferResource(size_t size);

	private:

		VkDevice m_Device;
		MemoryAllocator& m_Allocator;
	};
}