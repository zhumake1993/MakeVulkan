#include "VKBuffer.h"
#include "VulkanTools.h"

namespace vk
{
	BufferManager::BufferManager(VkDevice device, MemoryAllocator & allocator)
		: m_Device(device)
		, m_Allocator(allocator)
	{
	}

	BufferManager::~BufferManager()
	{

	}

	BufferResource * BufferManager::CreateBufferResource(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags)
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;
		VK_CHECK_RESULT(vkCreateBuffer(m_Device, &bufferCreateInfo, nullptr, &buffer));

		// todo：VK_KHR_dedicated_allocation， VK_KHR_get_memory_requirements2
		// 一些资源使用单独分配的内存，效率可能更高

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(m_Device, buffer, &memoryRequirements);

		int memoryTypeIndex = FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memFlags);
		ASSERT(memoryTypeIndex > -1);

		Memory memory = m_Allocator.Allocate(memoryTypeIndex, memoryRequirements.size, memoryRequirements.alignment);
		ASSERT(memory.IsValid());

		vkBindBufferMemory(m_Device, buffer, memory.memory, memory.offset);

		// todo：BufferView
		// 用于computer buffer

		return new BufferResource();
	}
}