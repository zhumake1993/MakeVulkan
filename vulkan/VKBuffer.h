#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "Buffer.h"
#include "VKMemory.h"

namespace vk
{
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

	class VulkanBuffer : public Buffer
	{

	public:

		/*BufferImpl(VKBuffer* buffer);
		virtual ~BufferImpl();

		VKBuffer* GetBuffer() { return m_Buffer; }
		void SetBuffer(VKBuffer* buffer) { m_Buffer = buffer; }*/

	private:

		//VKBuffer* m_Buffer;
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