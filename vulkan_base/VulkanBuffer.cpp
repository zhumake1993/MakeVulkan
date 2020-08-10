#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanBuffer::VulkanBuffer(VulkanDevice* vulkanDevice, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty):
	m_VulkanDevice(vulkanDevice),
	m_Size(size),
	m_Usage(usage),
	m_MemoryProperty(memoryProperty)
{
	CreateBuffer();
	AllocateMemory();
	VK_CHECK_RESULT(vkBindBufferMemory(m_VulkanDevice->m_LogicalDevice, m_Buffer, m_Memory, 0));
}

VulkanBuffer::~VulkanBuffer()
{
}

void VulkanBuffer::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE) {
		if (m_Buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(m_VulkanDevice->m_LogicalDevice, m_Buffer, nullptr);
			m_Buffer = VK_NULL_HANDLE;
		}
		if (m_Memory != VK_NULL_HANDLE) {
			vkFreeMemory(m_VulkanDevice->m_LogicalDevice, m_Memory, nullptr);
			m_Memory = VK_NULL_HANDLE;
		}
	}
}

void VulkanBuffer::MapAndCopy(void * data, uint32_t size)
{
	void* memoryPointer = Map(size);
	memcpy(memoryPointer, data, size);
	Unmap();
}

void * VulkanBuffer::Map(uint32_t size)
{
	void *bufferMemoryPointer;
	VK_CHECK_RESULT(vkMapMemory(m_VulkanDevice->m_LogicalDevice, m_Memory, 0, size, 0, &bufferMemoryPointer));
	return bufferMemoryPointer;
}

void VulkanBuffer::Unmap()
{
	// 如果使用了VK_MEMORY_PROPERTY_HOST_COHERENT_BIT，就不需要flush
	// size 必须是 VkPhysicalDeviceLimits::nonCoherentAtomSize 的倍数，这里为了方便就使用VK_WHOLE_SIZE
	VkMappedMemoryRange flushRange = {};
	flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	flushRange.pNext = nullptr;
	flushRange.memory = m_Memory;
	flushRange.offset = 0;
	flushRange.size = VK_WHOLE_SIZE;

	vkFlushMappedMemoryRanges(m_VulkanDevice->m_LogicalDevice, 1, &flushRange);

	vkUnmapMemory(m_VulkanDevice->m_LogicalDevice, m_Memory);
}

VkDescriptorBufferInfo VulkanBuffer::GetVkDescriptorBufferInfo()
{
	VkDescriptorBufferInfo descriptorBufferInfo = {};
	descriptorBufferInfo.buffer = m_Buffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = m_Size;

	return descriptorBufferInfo;
}

void VulkanBuffer::CreateBuffer()
{
	VkBufferCreateInfo vertexBufferInfo = {};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = nullptr;
	vertexBufferInfo.flags = 0;
	vertexBufferInfo.size = m_Size;
	vertexBufferInfo.usage = m_Usage;
	vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vertexBufferInfo.queueFamilyIndexCount = 0;
	vertexBufferInfo.pQueueFamilyIndices = nullptr;

	VK_CHECK_RESULT(vkCreateBuffer(m_VulkanDevice->m_LogicalDevice, &vertexBufferInfo, nullptr, &m_Buffer));
}

void VulkanBuffer::AllocateMemory()
{
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(m_VulkanDevice->m_LogicalDevice, m_Buffer, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = m_VulkanDevice->GetMemoryTypeIndex(memReqs.memoryTypeBits, m_MemoryProperty);
	VK_CHECK_RESULT(vkAllocateMemory(m_VulkanDevice->m_LogicalDevice, &memoryAllocateInfo, nullptr, &m_Memory));
}
