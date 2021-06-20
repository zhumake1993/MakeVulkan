#pragma once

#include "Env.h"
#include "NonCopyable.h"

namespace vk
{
	int FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags);

	struct Memory
	{
		VkDeviceMemory memory;
		VkDeviceSize offset;
		VkDeviceSize size;
		void* mapped;

		// 注意：该flags是实际的flags，跟申请内存时提供的flags可能不一致
		// 例如，申请内存时只要求VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT，但是实际的flags可能是VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		VkMemoryPropertyFlags flags;
		uint32_t memoryTypeIndex;
	};

	class MemoryAllocator : NonCopyable
	{
	public:

		MemoryAllocator(VkDevice device);
		virtual ~MemoryAllocator();

		Memory Allocate(int memoryTypeIndex, VkDeviceSize size, VkDeviceSize alignment);

	private:

		Memory AllocateImpl(int memoryTypeIndex, VkDeviceSize size, VkDeviceSize alignment);

	private:

		VkDevice m_Device;
	};
}