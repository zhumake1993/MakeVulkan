#include "VKBuffer.h"

#include "DeviceProperties.h"
#include "Tools.h"
#include "VulkanDriver.h"

#include "VKDevice.h"

VKBuffer::VKBuffer(VKDevice * vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty) :
	device(vkDevice->device)
{
	// buffer

	VkBufferCreateInfo vertexBufferInfo = {};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = nullptr;
	vertexBufferInfo.flags = 0;
	vertexBufferInfo.size = size;
	vertexBufferInfo.usage = usage;
	vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vertexBufferInfo.queueFamilyIndexCount = 0;
	vertexBufferInfo.pQueueFamilyIndices = nullptr;

	VK_CHECK_RESULT(vkCreateBuffer(device, &vertexBufferInfo, nullptr, &buffer));

	// Memory

	auto& dp = GetDeviceProperties();

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(device, buffer, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = dp.GetMemoryTypeIndex(memReqs.memoryTypeBits, memoryProperty);
	VK_CHECK_RESULT(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory));

	// Bind

	VK_CHECK_RESULT(vkBindBufferMemory(device, buffer, memory, 0));
}

VKBuffer::~VKBuffer()
{
	if (device != VK_NULL_HANDLE) {
		if (buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(device, buffer, nullptr);
			buffer = VK_NULL_HANDLE;
		}
		if (memory != VK_NULL_HANDLE) {
			vkFreeMemory(device, memory, nullptr);
			memory = VK_NULL_HANDLE;
		}
	}
}

void VKBuffer::Map(VkDeviceSize offset, VkDeviceSize size)
{
	assert(mappedPointer == nullptr);
	VK_CHECK_RESULT(vkMapMemory(device, memory, offset, size, 0, &mappedPointer));
}

void VKBuffer::Unmap()
{
	vkUnmapMemory(device, memory);
}

void VKBuffer::Copy(void * data, VkDeviceSize offset, VkDeviceSize size)
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
