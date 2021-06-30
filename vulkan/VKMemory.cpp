#include "VKMemory.h"
#include "DeviceProperties.h"
#include "VKTools.h"
#include "Tools.h"

namespace vk
{
	int FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags)
	{
		VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties = GetDeviceProperties().deviceMemoryProperties;

		// Search memtypes to find first index with those properties
		for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < VK_MAX_MEMORY_TYPES; ++memoryTypeIndex)
		{
			if ((memoryTypeBits & 1) == 1)
			{
				// Type is available, does it match user properties?
				if ((physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
					return memoryTypeIndex;
			}
			memoryTypeBits >>= 1;
		}

		return -1;
	}

	Memory AllocateMemory(VkDevice device, int memoryTypeIndex, VkDeviceSize size)
	{
		Memory memory = {};

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
		VK_CHECK_RESULT(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory.memory));

		memory.offset = 0;
		memory.size = size;
		memory.memoryTypeIndex = memoryTypeIndex;
		memory.flags = GetDeviceProperties().deviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;

		memory.mapped = nullptr;
		if (memory.flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			VK_CHECK_RESULT(vkMapMemory(device, memory.memory, 0, VK_WHOLE_SIZE, 0, &memory.mapped));
		}

		memory.block = nullptr;
		memory.chunk = nullptr;

		return memory;
	}

	void FreeMemory(VkDevice device, Memory& memory)
	{
		// Spec：If a memory object is mapped at the time it is freed, it is implicitly unmapped.
		// 这里保险起见
		if (memory.mapped)
			vkUnmapMemory(device, memory.memory);

		vkFreeMemory(device, memory.memory, nullptr);
	}

	// ===============================================================================
	// MemoryBlock
	// ===============================================================================

	MemoryBlock::MemoryBlock(VkDevice device, int memoryTypeIndex, VkDeviceSize blockSize, VkDeviceSize alignment)
		: m_Device(device)
		, m_Alignment(alignment)
	{
		m_Memory = AllocateMemory(m_Device, memoryTypeIndex, blockSize);
		m_Memory.block = this;

		m_Head = new Chunk();
		m_Head->offset = 0;
		m_Head->size = blockSize;
		m_Head->allocated = false;
		m_Head->prev = nullptr;
		m_Head->next = nullptr;

		m_leftSize = blockSize;
	}

	MemoryBlock::~MemoryBlock()
	{
		FreeMemory(m_Device, m_Memory);

		while (m_Head)
		{
			Chunk* old = m_Head;
			m_Head = m_Head->next;
			delete old;
		}
	}

	Memory MemoryBlock::Allocate(VkDeviceSize size, VkDeviceSize alignment)
	{
		Memory memory = {};

		if (size > m_leftSize)
			return memory;

		Chunk* currentChunk;
		VkDeviceSize alignedOffset = 0;
		VkDeviceSize alignedSize = 0;

		for (currentChunk = m_Head; currentChunk != nullptr; currentChunk = currentChunk->next)
		{
			if (currentChunk->allocated)
				continue;

			// 这两个alignment通常都是2的幂
			alignedOffset = ALIGN(currentChunk->offset, m_Alignment);
			alignedOffset = ALIGN(alignedOffset, alignment);

			alignedSize = alignedOffset - currentChunk->offset + size;

			if (alignedSize > currentChunk->size)
				continue;

			break;
		}

		if (currentChunk == nullptr)
			return memory;

		if (currentChunk->size > alignedSize)
		{
			Chunk* newChunk = new Chunk();
			newChunk->offset = currentChunk->offset + alignedSize;
			newChunk->size = currentChunk->size - alignedSize;
			newChunk->allocated = false;

			// 由 prev -> targetChunk -> next
			// 变为 prev -> targetChunk -> newChunk -> next

			Chunk* next = currentChunk->next;

			currentChunk->next = newChunk;
			newChunk->prev = currentChunk;

			newChunk->next = next;
			if (next)
				next->prev = newChunk;
		}

		currentChunk->size = alignedSize;
		currentChunk->allocated = true;

		m_leftSize -= alignedSize;

		memory = m_Memory;
		memory.offset = alignedOffset;
		memory.size = size;
		
		if (memory.mapped)
			memory.mapped = static_cast<char*>(memory.mapped) + alignedOffset;

		memory.chunk = static_cast<void*>(currentChunk);

		return memory;
	}

	void MemoryBlock::Free(Memory & memory)
	{
		Chunk* currentChunk = static_cast<Chunk*>(memory.chunk);
		ASSERT(currentChunk->allocated);

		currentChunk->allocated = false;

		m_leftSize += currentChunk->size;

		Chunk* prev = currentChunk->prev;
		if (prev && !prev->allocated)
		{
			prev->next = currentChunk->next;
			if(currentChunk->next)
				currentChunk->next->prev = prev;

			prev->size += currentChunk->size;

			delete currentChunk;
			currentChunk = prev;
		}

		Chunk* next = currentChunk->next;
		if (next && !next->allocated)
		{
			currentChunk->next = next->next;
			if (next->next)
			{
				next->next->prev = currentChunk;
			}

			currentChunk->size += next->size;

			delete next;
		}
	}

	int MemoryBlock::GetChunkNum()
	{
		int chunkNum = 0;
		Chunk* chunk = m_Head;
		while (chunk)
		{
			chunkNum++;
			chunk = chunk->next;
		}
		return chunkNum;
	}

	VkDeviceSize MemoryBlock::GetAllocatedSize()
	{
		VkDeviceSize allocatedSize = 0;
		Chunk* chunk = m_Head;
		while (chunk)
		{
			if (chunk->allocated)
				allocatedSize += chunk->size;

			chunk = chunk->next;
		}
		return allocatedSize;
	}

	void MemoryBlock::Print()
	{
		int chunkNum = GetChunkNum();
		VkDeviceSize allocatedSize = GetAllocatedSize();

		LOG("\t\tMemoryBlock, allocate: %llu/%llu(%f%%), chunk num: %d, chunks:", allocatedSize, m_Memory.size, 100.0f * allocatedSize / m_Memory.size, chunkNum);

		Chunk* chunk = m_Head;
		while (chunk)
		{
			LOG(" [%llu,%llu,%llu,%d]", chunk->offset, chunk->size, chunk->offset + chunk->size, (chunk->allocated ? 1 : 0));

			chunk = chunk->next;
		}
		LOG("\n");
	}

	// ===============================================================================
	// MemoryTypeAllocator
	// ===============================================================================

	MemoryTypeAllocator::MemoryTypeAllocator(VkDevice device, int memoryTypeIndex, VkDeviceSize blockSize, VkDeviceSize alignment)
		: m_Device(device)
		, m_MemoryTypeIndex(memoryTypeIndex)
		, m_BlockSize(blockSize)
		, m_Alignment(alignment)
	{
		m_MemoryBlocks.push_back(new MemoryBlock(m_Device, m_MemoryTypeIndex, m_BlockSize, alignment));
	}

	MemoryTypeAllocator::~MemoryTypeAllocator()
	{
		for (auto itr = m_MemoryBlocks.begin(); itr != m_MemoryBlocks.end(); itr++)
		{
			delete *itr;
		}
	}

	Memory MemoryTypeAllocator::Allocate(VkDeviceSize size, VkDeviceSize alignment)
	{
		for (auto itr = m_MemoryBlocks.begin(); itr != m_MemoryBlocks.end(); itr++)
		{
			Memory memory = (*itr)->Allocate(size, alignment);
			if (memory.IsValid())
			{
				return memory;
			}
		}

		MemoryBlock* memoryBlock = new MemoryBlock(m_Device, m_MemoryTypeIndex, m_BlockSize, alignment);
		m_MemoryBlocks.push_back(memoryBlock);

		return memoryBlock->Allocate(size, alignment);
	}

	void MemoryTypeAllocator::Free(Memory & memory)
	{
		memory.block->Free(memory);

		// 当该MemoryBlock完全free时，可以释放它，以减少内存
		// 不过这里我选择保留，因为更注重速度
	}

	int MemoryTypeAllocator::GetBlockNum()
	{
		return static_cast<int>(m_MemoryBlocks.size());
	}

	VkDeviceSize MemoryTypeAllocator::GetAllocatedSize()
	{
		VkDeviceSize allocatedSize = 0;
		for (auto itr = m_MemoryBlocks.begin(); itr != m_MemoryBlocks.end(); itr++)
		{
			allocatedSize += (*itr)->GetAllocatedSize();
		}
		return allocatedSize;
	}

	VkDeviceSize MemoryTypeAllocator::GetTotalSize()
	{
		return GetBlockNum() * m_BlockSize;
	}

	void MemoryTypeAllocator::Print()
	{
		int blockNum = GetBlockNum();
		VkDeviceSize allocatedSize = GetAllocatedSize();
		VkDeviceSize totalSize = GetTotalSize();

		LOG("\tMemoryTypeAllocator, memoryTypeIndex: %d, allocate: %llu/%llu(%f%%), block num: %d, blocks:\n", m_MemoryTypeIndex, allocatedSize, totalSize, 100.0f * allocatedSize / totalSize, blockNum);
		for (auto itr = m_MemoryBlocks.begin(); itr != m_MemoryBlocks.end(); itr++)
		{
			(*itr)->Print();
		}
	}

	// ===============================================================================
	// MemoryAllocator
	// ===============================================================================

	MemoryAllocator::MemoryAllocator(VkDevice device, VkDeviceSize blockSize, VkDeviceSize alignment)
		: m_Device(device)
		, m_BlockSize(blockSize)
		, m_Alignment(alignment)
	{
	}

	MemoryAllocator::~MemoryAllocator()
	{
		for (int i = 0; i < VK_MAX_MEMORY_TYPES; i++)
		{
			if (m_MemoryTypeAllocators[i])
				delete m_MemoryTypeAllocators[i];
		}
	}

	Memory MemoryAllocator::Allocate(int memoryTypeIndex, VkDeviceSize size, VkDeviceSize alignment)
	{
		if (!m_MemoryTypeAllocators[memoryTypeIndex])
		{
			m_MemoryTypeAllocators[memoryTypeIndex] = new MemoryTypeAllocator(m_Device, memoryTypeIndex, m_BlockSize, alignment);
		}

		return m_MemoryTypeAllocators[memoryTypeIndex]->Allocate(size, alignment);
	}

	Memory MemoryAllocator::AllocateDedicated(int memoryTypeIndex, VkDeviceSize size)
	{
		Memory memory = AllocateMemory(m_Device, memoryTypeIndex, size);

		return memory;
	}

	void MemoryAllocator::Free(Memory & memory)
	{
		if (memory.block)
		{
			ASSERT(m_MemoryTypeAllocators[memory.memoryTypeIndex]);
			m_MemoryTypeAllocators[memory.memoryTypeIndex]->Free(memory);
		}
		else
		{
			FreeMemory(m_Device, memory);
		}
	}

	VkDeviceSize MemoryAllocator::GetAllocatedSize()
	{
		VkDeviceSize allocatedSize = 0;
		for (int i = 0; i < VK_MAX_MEMORY_TYPES; i++)
		{
			if (m_MemoryTypeAllocators[i])
			{
				allocatedSize += m_MemoryTypeAllocators[i]->GetAllocatedSize();
			}
		}
		return allocatedSize;
	}

	VkDeviceSize MemoryAllocator::GetTotalSize()
	{
		VkDeviceSize totalSize = 0;
		for (int i = 0; i < VK_MAX_MEMORY_TYPES; i++)
		{
			if (m_MemoryTypeAllocators[i])
			{
				totalSize += m_MemoryTypeAllocators[i]->GetTotalSize();
			}
		}
		return totalSize;
	}

	void MemoryAllocator::Print()
	{
		VkDeviceSize allocatedSize = GetAllocatedSize();
		VkDeviceSize totalSize = GetTotalSize();

		LOG("MemoryAllocator, blockSiize: %llu, alignment: %llu, allocate: %llu/%llu(%f%%)\n", m_BlockSize, m_Alignment, allocatedSize, totalSize, 100.0f * allocatedSize / totalSize);
		for (int i = 0; i < VK_MAX_MEMORY_TYPES; i++)
		{
			if (m_MemoryTypeAllocators[i])
			{
				m_MemoryTypeAllocators[i]->Print();
			}
		}

		// todo：使用AllocateDedicated的内存也需要被追踪
		// 另外，其他资源的内存后面也要使用MemoryAllocator
	}
}