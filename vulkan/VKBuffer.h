#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "GfxBuffer.h"
#include "VKMemory.h"
#include "GfxTypes.h"

namespace vk
{
	class BufferManager;

	class BufferResource : public VKResource
	{
		/*VKBuffer(VkDevice vkDevice, VkDeviceSize vkSize, VkBufferUsageFlags vkUsage, VkMemoryPropertyFlags vkMemoryProperty) :
			device(vkDevice), size(vkSize), usage(vkUsage), memoryProperty(vkMemoryProperty)
		{
		}
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

		VkDevice device = VK_NULL_HANDLE;*/
	};

	class VulkanBuffer : public GfxBuffer
	{

	public:

		VulkanBuffer(BufferManager* bufferManager, GfxBufferUsage bufferUsage, GfxBufferMode bufferMode, uint64_t size);
		virtual ~VulkanBuffer();

		void Update(void* data, uint64_t offset, uint64_t size);

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

	private:

		VkDevice m_Device;
		MemoryAllocator& m_Allocator;
	};
}