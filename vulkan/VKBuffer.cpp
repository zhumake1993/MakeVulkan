#include "VKBuffer.h"
#include "VKTools.h"
#include "Tools.h"
#include "VKDeviceProperties.h"
#include "VKCommandBuffer.h"

namespace vk
{
	// ===============================================================================
	// BufferResource
	// ===============================================================================

	BufferResource::BufferResource(VkDevice device, MemoryAllocator& allocator, VkBuffer buffer, Memory memory)
		: m_Device(device)
		, m_Allocator(allocator)
		, m_Buffer(buffer)
		, m_Memory(memory)
	{
	}

	BufferResource::~BufferResource()
	{
		vkDestroyBuffer(m_Device, m_Buffer, nullptr);
		m_Allocator.Free(m_Memory);
	}

	VkBuffer BufferResource::GetBuffer()
	{
		return m_Buffer;
	}

	VkBuffer BufferResource::AccessBuffer()
	{
		Use();

		return m_Buffer;
	}

	void BufferResource::Update(void * data, uint64_t offset, uint64_t size, VKCommandBuffer* cmdBuffer, uint64_t frameIndex, BufferManager* bufferManager)
	{
		if (m_Memory.flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			memcpy(static_cast<char*>(m_Memory.mapped) + offset, data, size);

			Flush(offset, size);
		}
		else
		{
			BufferResource* stagingBuffer = bufferManager->CreateTransientStagingBufferResource(size);
			stagingBuffer->Update(data, 0, size);

			cmdBuffer->Begin();

			VkBufferCopy bufferCopyInfo = {};
			bufferCopyInfo.srcOffset = 0;
			bufferCopyInfo.dstOffset = offset;
			bufferCopyInfo.size = size;
			cmdBuffer->CopyBuffer(stagingBuffer->GetBuffer(), m_Buffer, bufferCopyInfo);

			// todo
			// 经测试发现没有这一步也没问题（许多教程也的确没有这一步）
			// 个人认为是因为调用了DeviceWaitIdle
			//vkCmdPipelineBarrier

			cmdBuffer->End();

			// todo
			cmdBuffer->m_NeedSubmit = true;
		}
	}

	void BufferResource::Flush(VkDeviceSize offset, VkDeviceSize size)
	{
		if ((m_Memory.flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange range = MakeMappedMemoryRange(offset, size);

			vkFlushMappedMemoryRanges(m_Device, 1, &range);
		}
	}

	void BufferResource::Invalidate(VkDeviceSize offset, VkDeviceSize size)
	{
		if ((m_Memory.flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange range = MakeMappedMemoryRange(offset, size);

			vkInvalidateMappedMemoryRanges(m_Device, 1, &range);
		}
	}

	VkMappedMemoryRange BufferResource::MakeMappedMemoryRange(VkDeviceSize offset, VkDeviceSize size)
	{
		VkMappedMemoryRange range = {};
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.pNext = nullptr;
		range.memory = m_Memory.memory;
		range.offset = m_Memory.offset + offset;

		if (size == VK_WHOLE_SIZE)
			range.size = m_Memory.size - offset;
		else
			range.size = size;

		ASSERT(range.offset % vk::GetVKDeviceProperties().deviceProperties.limits.nonCoherentAtomSize == 0);
		ASSERT(range.size % vk::GetVKDeviceProperties().deviceProperties.limits.nonCoherentAtomSize == 0);

		return range;
	}

	// ===============================================================================
	// VulkanBuffer
	// ===============================================================================

	VulkanBuffer::VulkanBuffer(BufferManager* bufferManager, GfxBufferUsage bufferUsage, GfxBufferMode bufferMode, uint64_t size)
		: GfxBuffer(bufferUsage, bufferMode, size)
		, m_BufferManager(bufferManager)
	{
		m_BufferResource = CreateBufferResource();
	}

	VulkanBuffer::~VulkanBuffer()
	{
		m_BufferResource->Release();
	}

	VkBuffer VulkanBuffer::GetBuffer()
	{
		return m_BufferResource->GetBuffer();
	}

	VkBuffer VulkanBuffer::AccessBuffer()
	{
		return m_BufferResource->AccessBuffer();
	}

	void VulkanBuffer::Update(void* data, uint64_t offset, uint64_t size, VKCommandBuffer* cmdBuffer, uint64_t frameIndex)
	{
		if (m_BufferResource->InUse())
		{
			m_BufferResource->Release();

			m_BufferResource = CreateBufferResource();
		}

		m_BufferResource->Update(data, offset, size, cmdBuffer, frameIndex, m_BufferManager);
	}

	BufferResource * VulkanBuffer::CreateBufferResource()
	{
		VkBufferUsageFlags usage = 0;
		VkMemoryPropertyFlags memoryProperty = 0;

		switch (m_BufferUsage)
		{
		case kGfxBufferUsageVertex:
			usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			break;
		case kGfxBufferUsageIndex:
			usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			break;
		case kGfxBufferUsageUniform:
			usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			break;
		default:LOGE("Wrong GfxBufferUsage");
		}

		switch (m_BufferMode)
		{
		case kGfxBufferModeDeviceLocal:
			memoryProperty |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			break;
		case kGfxBufferModeHostVisible:
			memoryProperty |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			break;
		default:LOGE("Wrong MemoryPropertyType");
		}

		return m_BufferManager->CreateBufferResource(m_Size, usage, memoryProperty);
	}

	// ===============================================================================
	// BufferManager
	// ===============================================================================

	BufferManager::BufferManager(VkDevice device, MemoryAllocator & allocator)
		: m_Device(device)
		, m_Allocator(allocator)
	{
	}

	BufferManager::~BufferManager()
	{

	}

	BufferResource * BufferManager::CreateBufferResource(uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags)
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

		return new BufferResource(m_Device, m_Allocator, buffer, memory);
	}

	BufferResource * BufferManager::CreateTransientStagingBufferResource(uint64_t size)
	{
		BufferResource* stagingBuffer = CreateBufferResource(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		stagingBuffer->Use();
		stagingBuffer->Release();

		return stagingBuffer;
	}
	BufferResource * BufferManager::CreateTransientUniformBufferResource(uint64_t size)
	{
		BufferResource* stagingBuffer = CreateBufferResource(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		stagingBuffer->Use();
		stagingBuffer->Release();

		return stagingBuffer;
	}
}