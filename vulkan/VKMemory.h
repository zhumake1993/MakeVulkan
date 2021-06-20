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

		// ע�⣺��flags��ʵ�ʵ�flags���������ڴ�ʱ�ṩ��flags���ܲ�һ��
		// ���磬�����ڴ�ʱֻҪ��VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT������ʵ�ʵ�flags������VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
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