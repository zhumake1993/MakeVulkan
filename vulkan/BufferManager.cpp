#include "BufferManager.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"
#include "GarbageCollector.h"

void VKBuffer::Map(VkDeviceSize offset, VkDeviceSize size)
{
	assert(mappedPointer == nullptr);
	VK_CHECK_RESULT(vkMapMemory(device, memory, offset, size, 0, &mappedPointer));
}

void VKBuffer::Unmap()
{
	vkUnmapMemory(device, memory);
}

void VKBuffer::Update(void * data, VkDeviceSize offset, VkDeviceSize size)
{
	assert(mappedPointer);
	memcpy(static_cast<char*>(mappedPointer) + offset, data, size);
}

void VKBuffer::Flush(VkDeviceSize offset, VkDeviceSize size)
{
	// only required for non-coherent memory
	VkMappedMemoryRange flushRange = {};
	flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	flushRange.pNext = nullptr;
	flushRange.memory = memory;
	flushRange.offset = offset;
	flushRange.size = size;

	vkFlushMappedMemoryRanges(device, 1, &flushRange);
}

void VKBuffer::Invalidate(VkDeviceSize offset, VkDeviceSize size)
{
	// only required for non-coherent memory
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.pNext = nullptr;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;

	vkInvalidateMappedMemoryRanges(device, 1, &mappedRange);
}

BufferManager::BufferManager(VkDevice vkDevice, GarbageCollector* gc) :
	m_Device(vkDevice),
	m_GarbageCollector(gc)
{
	m_StagingBuffer = CreateBuffer(m_StagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

BufferManager::~BufferManager()
{
	RELEASE(m_StagingBuffer);
}

VKBuffer * BufferManager::GetStagingBuffer()
{
	return m_StagingBuffer;
}

VKBuffer * BufferManager::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty)
{
	VKBuffer* buffer = CreateBufferInternal(size, usage, memoryProperty);

	return buffer;
}

VKBuffer * BufferManager::CreateTempBuffer(VkDeviceSize size)
{
	VKBuffer* buffer = CreateBufferInternal(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	m_GarbageCollector->AddResource(buffer);

	return buffer;
}

VKBuffer * BufferManager::CreateBufferInternal(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty)
{
	VKBuffer* buffer = new VKBuffer(m_Device, size, usage, memoryProperty);

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

	VK_CHECK_RESULT(vkCreateBuffer(m_Device, &vertexBufferInfo, nullptr, &buffer->buffer));

	// Memory

	auto& dp = GetDeviceProperties();

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(m_Device, buffer->buffer, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = dp.GetMemoryTypeIndex(memReqs.memoryTypeBits, memoryProperty);
	VK_CHECK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &buffer->memory));

	// Bind

	VK_CHECK_RESULT(vkBindBufferMemory(m_Device, buffer->buffer, buffer->memory, 0));

	// Map

	if (memoryProperty & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		buffer->Map();
	}

	return buffer;
}
