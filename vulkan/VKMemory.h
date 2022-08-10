#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "mkArray.h"
#include "mkList.h"

namespace vk
{
	int FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags);

	class MemoryBlock;
	struct Memory
	{
		VkDeviceMemory memory;
		VkDeviceSize offset;
		VkDeviceSize size; // ���ڶ��룬���ܱȶ�Ӧ��Chunk��sizeС
		void* mapped; // ƫ��offset���ָ��

		// ע�⣺��flags��ʵ�ʵ�flags���������ڴ�ʱ�ṩ��flags���ܲ�һ��
		// ���磬�����ڴ�ʱֻҪ��VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT������ʵ�ʵ�flags������VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
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

		int GetChunkNum();
		VkDeviceSize GetAllocatedSize();

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

		int GetBlockNum();
		VkDeviceSize GetAllocatedSize();
		VkDeviceSize GetTotalSize();

		void Print();

	private:

		int m_MemoryTypeIndex;
		VkDeviceSize m_BlockSize;
		VkDeviceSize m_Alignment;
		mkList<MemoryBlock*> m_MemoryBlocks;

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

		VkDeviceSize GetAllocatedSize();
		VkDeviceSize GetTotalSize();

		void Print();

	private:

		VkDeviceSize m_BlockSize;
		VkDeviceSize m_Alignment;
		mkArray<MemoryTypeAllocator*, VK_MAX_MEMORY_TYPES> m_MemoryTypeAllocators;

		VkDevice m_Device;
	};
}