#include "BufferManager.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"
#include "VKCommandBuffer.h"

VKBuffer::VKBuffer(BufferType bufferType, uint32_t currFrameIndex, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty, bool persistent) :
	Buffer(bufferType),
	m_Device(vkDevice),
	m_Size(size),
	m_Usage(usage),
	m_MemoryProperty(memoryProperty),
	m_Persistent(persistent)
{
	m_BufferResource = new VKBufferResource(currFrameIndex);

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

	VK_CHECK_RESULT(vkCreateBuffer(m_Device, &vertexBufferInfo, nullptr, &m_BufferResource->buffer));

	// Memory

	auto& dp = GetDeviceProperties();

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(m_Device, m_BufferResource->buffer, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = dp.GetMemoryTypeIndex(memReqs.memoryTypeBits, memoryProperty);
	VK_CHECK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &m_BufferResource->memory));

	// Bind

	VK_CHECK_RESULT(vkBindBufferMemory(m_Device, m_BufferResource->buffer, m_BufferResource->memory, 0));

	// Map

	if (memoryProperty & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		VK_CHECK_RESULT(vkMapMemory(m_Device, m_BufferResource->memory, 0, size, 0, &m_BufferResource->mappedPointer));
	}
}

VKBuffer::~VKBuffer()
{
	vkDestroyBuffer(m_Device, m_BufferResource->buffer, nullptr);
	vkFreeMemory(m_Device, m_BufferResource->memory, nullptr);
	RELEASE(m_BufferResource);
}

bool VKBuffer::InUse(uint32_t currFrameIndex)
{
	return m_BufferResource->InUse(currFrameIndex);
}

void VKBuffer::Map(VkDeviceSize offset, VkDeviceSize size)
{
	assert(m_BufferResource->mappedPointer == nullptr);
	VK_CHECK_RESULT(vkMapMemory(m_Device, m_BufferResource->memory, offset, size, 0, &m_BufferResource->mappedPointer));
}

void VKBuffer::Unmap()
{
	vkUnmapMemory(m_Device, m_BufferResource->memory);
}

void VKBuffer::Update(void * data, VkDeviceSize offset, VkDeviceSize size)
{
	assert(m_BufferResource->mappedPointer);
	memcpy(static_cast<char*>(m_BufferResource->mappedPointer) + offset, data, size);
}

void VKBuffer::Flush(VkDeviceSize offset, VkDeviceSize size)
{
	// only required for non-coherent memory
	VkMappedMemoryRange flushRange = {};
	flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	flushRange.pNext = nullptr;
	flushRange.memory = m_BufferResource->memory;
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
	mappedRange.memory = m_BufferResource->memory;
	mappedRange.offset = offset;
	mappedRange.size = size;

	vkInvalidateMappedMemoryRanges(m_Device, 1, &mappedRange);
}

void VKBuffer::SwitchResource(VKBuffer * other)
{
	VKBufferResource* temp = m_BufferResource;
	m_BufferResource = other->m_BufferResource;
	other->m_BufferResource = temp;
}

BufferManager::BufferManager(VkDevice vkDevice) :
	m_Device(vkDevice)
{
	m_StagingBuffer = CreateBuffer(kBufferTypeStaging, m_StagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, true);
}

BufferManager::~BufferManager()
{
	//vkDestroyBuffer(m_Device, m_StagingBuffer->m_Buffer->buffer, nullptr);
	//vkFreeMemory(m_Device, m_StagingBuffer->m_Buffer->memory, nullptr);
	RELEASE(m_StagingBuffer);

	for (auto itr = m_PendingBuffers.begin(); itr != m_PendingBuffers.end(); itr++)
	{
		//vkDestroyBuffer(m_Device, (*itr)->m_Buffer->buffer, nullptr);
		//vkFreeMemory(m_Device, (*itr)->m_Buffer->memory, nullptr);
		RELEASE(*itr);
	}
	m_PendingBuffers.clear();
}

void BufferManager::Update()
{
	// 找到第一个可以被销毁的Buffer
	auto unused = m_PendingBuffers.begin();
	for (; unused != m_PendingBuffers.end(); unused++)
	{
		if (!(*unused)->InUse(m_FrameIndex))
		{
			break;
		}
	}

	for (auto itr = unused; itr != m_PendingBuffers.end(); itr++)
	{
		//vkDestroyBuffer(m_Device, (*itr)->m_Buffer->buffer, nullptr);
		//vkFreeMemory(m_Device, (*itr)->m_Buffer->memory, nullptr);
		RELEASE(*itr);
	}

	m_PendingBuffers.erase(unused, m_PendingBuffers.end());

	// 新的Buffer放在list前部
	m_PendingBuffers.splice(m_PendingBuffers.begin(), m_NewBuffers);
	m_NewBuffers.clear();

	m_FrameIndex++;
}

VKBuffer * BufferManager::GetStagingBuffer()
{
	return m_StagingBuffer;
}

VKBuffer * BufferManager::CreateBuffer(BufferType bufferType, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty, bool persistent)
{
	VKBuffer* buffer = new VKBuffer(bufferType, m_FrameIndex, m_Device, size, usage, memoryProperty, persistent);

	if (!persistent)
	{
		m_NewBuffers.push_back(buffer);
	}

	return buffer;
}

void BufferManager::UpdateBuffer(VKBuffer* buffer, void * data, uint64_t size, VKCommandBuffer * commandBuffer)
{
	if (buffer->InUse(m_FrameIndex))
	{
		VKBuffer* dummyBuffer = CreateBuffer(buffer->GetBufferType(), buffer->GetSize(), buffer->GetUsage(), buffer->GetMemoryProperty(), buffer->GetPersistent());

		dummyBuffer->SwitchResource(buffer);

		m_NewBuffers.push_back(dummyBuffer);

		//newBuffer = CreateBuffer(buffer->GetBufferType(), buffer->GetSize(), buffer->GetUsage(), buffer->GetMemoryProperty(), buffer->GetPersistent());
	}
	/*else
	{
		newBuffer = buffer;
	}*/

	if (commandBuffer)
	{
		m_StagingBuffer->Update(data, 0, size);
		//memcpy(static_cast<char*>(m_StagingBuffer->m_Buffer->mappedPointer), data, size);

		commandBuffer->Begin();

		VkBufferCopy bufferCopyInfo = {};
		bufferCopyInfo.srcOffset = 0;
		bufferCopyInfo.dstOffset = 0;
		bufferCopyInfo.size = size;
		commandBuffer->CopyBuffer(m_StagingBuffer->GetBuffer(), buffer->GetBuffer(), bufferCopyInfo);

		// 经测试发现没有这一步也没问题（许多教程也的确没有这一步）
		// 个人认为是因为调用了DeviceWaitIdle
		//vkCmdPipelineBarrier

		commandBuffer->End();
	}
	else
	{
		buffer->Update(data, 0, size);
		//memcpy(static_cast<char*>(newBuffer->m_Buffer->mappedPointer), data, size);
	}

	//return newBuffer;
}