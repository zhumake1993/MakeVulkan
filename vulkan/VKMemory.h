#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "MkArray.h"

namespace vk
{
	int FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags);

	struct Memory
	{
		VkDeviceMemory memory;
		VkDeviceSize offset;
		VkDeviceSize size; // 由于对齐，可能比对应的Chunk的size小
		void* mapped;

		// 注意：该flags是实际的flags，跟申请内存时提供的flags可能不一致
		// 例如，申请内存时只要求VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT，但是实际的flags可能是VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		VkMemoryPropertyFlags flags;
		uint32_t memoryTypeIndex;

		MemoryBlock* block;
		void* chunk;

		bool IsValid()
		{
			return memory != VK_NULL_HANDLE;
		}
	};

	class MemoryBlock : NonCopyable
	{
	public:

		MemoryBlock(VkDevice device, int memoryTypeIndex, VkDeviceSize blockSize, VkDeviceSize alignment);
		~MemoryBlock();

		Memory Allocate(VkDeviceSize size, VkDeviceSize alignment);
		void Free(Memory& memory);

		void Print();

	private:

		struct Chunk
		{
			VkDeviceSize offset;
			VkDeviceSize size;
			bool allocated;
			Chunk* prev;
			Chunk* next;
		};

	private:

		Memory m_Memory;
		Chunk* m_Head;
		VkDeviceSize m_leftSize;

		VkDevice m_Device;
		VkDeviceSize m_Alignment;
	};

	class MemoryTypeAllocator : NonCopyable
	{
	public:

		MemoryTypeAllocator(VkDevice device, int memoryTypeIndex, VkDeviceSize blockSize, VkDeviceSize alignment);
		~MemoryTypeAllocator();

		Memory Allocate(VkDeviceSize size, VkDeviceSize alignment);
		void Free(Memory& memory);

		void Print();

	private:

		int m_MemoryTypeIndex;
		VkDeviceSize m_BlockSize;
		VkDeviceSize m_Alignment;
		std::list<MemoryBlock*> m_MemoryBlocks;

		VkDevice m_Device;
	};

	class MemoryAllocator : NonCopyable
	{
	public:

		MemoryAllocator(VkDevice device, VkDeviceSize blockSize, VkDeviceSize alignment);
		~MemoryAllocator();

		Memory Allocate(int memoryTypeIndex, VkDeviceSize size, VkDeviceSize alignment);
		Memory AllocateDedicated(int memoryTypeIndex, VkDeviceSize size);
		void Free(Memory& memory);

		void Print();

	private:

		VkDeviceSize m_BlockSize;
		VkDeviceSize m_Alignment;
		MkArray<MemoryTypeAllocator*, VK_MAX_MEMORY_TYPES> m_MemoryTypeAllocators;

		VkDevice m_Device;
	};
}