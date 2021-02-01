#include "VKBuffer.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"
#include "VKGarbageCollector.h"

VKBuffer::VKBuffer(uint32_t currFrameIndex, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty) :
	VKResource(currFrameIndex),
	m_Device(vkDevice)
{
	// Buffer

	VkBufferCreateInfo vertexBufferInfo = {};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = nullptr;
	vertexBufferInfo.flags = 0;
	vertexBufferInfo.size = size;
	vertexBufferInfo.usage = usage;
	vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vertexBufferInfo.queueFamilyIndexCount = 0;
	vertexBufferInfo.pQueueFamilyIndices = nullptr;

	VK_CHECK_RESULT(vkCreateBuffer(m_Device, &vertexBufferInfo, nullptr, &m_Buffer));

	// Memory

	auto& dp = GetDeviceProperties();

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = dp.GetMemoryTypeIndex(memReqs.memoryTypeBits, memoryProperty);
	VK_CHECK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &m_Memory));

	// Bind

	VK_CHECK_RESULT(vkBindBufferMemory(m_Device, m_Buffer, m_Memory, 0));
}

VKBuffer::~VKBuffer()
{
	if (m_Device != VK_NULL_HANDLE)
	{
		if (m_Buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(m_Device, m_Buffer, nullptr);
			m_Buffer = VK_NULL_HANDLE;
		}
		if (m_Memory != VK_NULL_HANDLE)
		{
			vkFreeMemory(m_Device, m_Memory, nullptr);
			m_Memory = VK_NULL_HANDLE;
		}
	}
}

void VKBuffer::Map(VkDeviceSize offset, VkDeviceSize size)
{
	assert(m_MappedPointer == nullptr);
	VK_CHECK_RESULT(vkMapMemory(m_Device, m_Memory, offset, size, 0, &m_MappedPointer));
}

void VKBuffer::Unmap()
{
	vkUnmapMemory(m_Device, m_Memory);
}

void VKBuffer::Update(void * data, VkDeviceSize offset, VkDeviceSize size)
{
	assert(m_MappedPointer);
	memcpy(static_cast<char*>(m_MappedPointer) + offset, data, size);
}

void VKBuffer::Flush(VkDeviceSize offset, VkDeviceSize size)
{
	// only required for non-coherent memory
	VkMappedMemoryRange flushRange = {};
	flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	flushRange.pNext = nullptr;
	flushRange.memory = m_Memory;
	flushRange.offset = offset;
	flushRange.size = size;

	vkFlushMappedMemoryRanges(m_Device, 1, &flushRange);
}

void VKBuffer::Invalidate(VkDeviceSize offset, VkDeviceSize size)
{
	// only required for non-coherent memory
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.pNext = nullptr;
	mappedRange.memory = m_Memory;
	mappedRange.offset = offset;
	mappedRange.size = size;

	vkInvalidateMappedMemoryRanges(m_Device, 1, &mappedRange);
}

VKBufferResource::VKBufferResource(BufferType bufferType, VKGarbageCollector* gc, uint32_t currFrameIndex, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty) :
	Buffer(bufferType),
	m_VKGarbageCollector(gc),
	m_Size(size),
	m_Usage(usage),
	m_MemoryProperty(memoryProperty)
{
	m_Buffer = new VKBuffer(currFrameIndex, vkDevice, size, usage, memoryProperty);
	m_Buffer->Map();
}

VKBufferResource::~VKBufferResource()
{
	m_VKGarbageCollector->AddBuffers(m_Buffer);
}

void VKBufferResource::Update(uint32_t currFrameIndex, void * data, VkDeviceSize offset, VkDeviceSize size)
{
	if (m_Buffer->InUse(currFrameIndex))
	{
		m_VKGarbageCollector->AddBuffers(m_Buffer);

		m_Buffer = new VKBuffer(currFrameIndex, m_Buffer->m_Device, m_Size, m_Usage, m_MemoryProperty);

		m_Buffer->Map();
		m_Buffer->Update(data, offset, size);
	}
	else
	{
		m_Buffer->Update(data, offset, size);
	}
}

VKBuffer* VKBufferResource::GetVKBuffer()
{
	return m_Buffer;
}