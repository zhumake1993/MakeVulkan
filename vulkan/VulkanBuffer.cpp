#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanBuffer::VulkanBuffer(VulkanDevice* vulkanDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty):
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

void VulkanBuffer::Map(VkDeviceSize offset, VkDeviceSize size)
{
	assert(m_MappedPointer == nullptr);
	VK_CHECK_RESULT(vkMapMemory(m_VulkanDevice->m_LogicalDevice, m_Memory, offset, size, 0, &m_MappedPointer));
}

void VulkanBuffer::Unmap()
{
	vkUnmapMemory(m_VulkanDevice->m_LogicalDevice, m_Memory);
}

void VulkanBuffer::Copy(void * data, VkDeviceSize offset, VkDeviceSize size)
{
	assert(m_MappedPointer);
	memcpy(static_cast<char*>(m_MappedPointer) + offset, data, size);
}

void VulkanBuffer::Flush(VkDeviceSize offset, VkDeviceSize size)
{
	// ֻ��non-coherent����Ҫflush
	// size ������ VkPhysicalDeviceLimits::nonCoherentAtomSize �ı���
	VkMappedMemoryRange flushRange = {};
	flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	flushRange.pNext = nullptr;
	flushRange.memory = m_Memory;
	flushRange.offset = offset;
	flushRange.size = size;

	vkFlushMappedMemoryRanges(m_VulkanDevice->m_LogicalDevice, 1, &flushRange);
}

void VulkanBuffer::Invalidate(VkDeviceSize offset, VkDeviceSize size)
{
	// ֻ��non-coherent����Ҫinvalidate
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.pNext = nullptr;
	mappedRange.memory = m_Memory;
	mappedRange.offset = offset;
	mappedRange.size = size;

	vkInvalidateMappedMemoryRanges(m_VulkanDevice->m_LogicalDevice, 1, &mappedRange);
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
