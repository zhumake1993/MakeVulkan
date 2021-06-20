#include "VKMemory.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"

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

	MemoryAllocator::MemoryAllocator(VkDevice device)
		: m_Device(device)
	{
	}

	MemoryAllocator::~MemoryAllocator()
	{
	}

	Memory MemoryAllocator::Allocate(int memoryTypeIndex, VkDeviceSize size, VkDeviceSize alignment)
	{
		//todo,hard...
		return Memory();
	}

	Memory MemoryAllocator::AllocateImpl(int memoryTypeIndex, VkDeviceSize size, VkDeviceSize alignment)
	{
		Memory memory = {};

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
		VK_CHECK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &memory.memory));

		memory.offset = 0;
		memory.size = size;
		memory.memoryTypeIndex = memoryTypeIndex;
		memory.flags = GetDeviceProperties().deviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;
		memory.mapped = nullptr;

		if (memory.flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			VK_CHECK_RESULT(vkMapMemory(m_Device, memory.memory, 0, VK_WHOLE_SIZE, 0, &memory.mapped));
		}

		return memory;
	}
}