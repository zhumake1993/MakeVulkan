#include "VKBuffer.h"
#include "VKTools.h"
#include "Tools.h"

namespace vk
{
	// ===============================================================================
	// BufferResource
	// ===============================================================================

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
		
	}

	void VulkanBuffer::Update(void * data, uint64_t offset, uint64_t size)
	{
		// start
		if (vkBuffer->InUse())
		{
			m_GarbageCollector->AddResource(vkBuffer);

			vkBuffer = m_BufferManager->CreateBuffer(vkBuffer->size, vkBuffer->usage, vkBuffer->memoryProperty);

			bufferImpl->SetBuffer(vkBuffer);
		}

		if (vkBuffer->memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		{
			VKBuffer* stagingBuffer = m_BufferManager->GetStagingBuffer();
			stagingBuffer->Update(data, 0, size);

			m_UploadCommandBuffer->Begin();

			VkBufferCopy bufferCopyInfo = {};
			bufferCopyInfo.srcOffset = 0;
			bufferCopyInfo.dstOffset = offset;
			bufferCopyInfo.size = size;
			m_UploadCommandBuffer->CopyBuffer(stagingBuffer->buffer, vkBuffer->buffer, bufferCopyInfo);

			// 经测试发现没有这一步也没问题（许多教程也的确没有这一步）
			// 个人认为是因为调用了DeviceWaitIdle
			//vkCmdPipelineBarrier

			m_UploadCommandBuffer->End();

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_UploadCommandBuffer->commandBuffer;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
			VK_CHECK_RESULT(vkQueueSubmit(m_VKContex->queue, 1, &submitInfo, VK_NULL_HANDLE));
		}
		else
		{
			vkBuffer->Update(data, offset, size);
		}
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

		// 先测试其他代码
		//Memory memory = m_Allocator.Allocate(memoryTypeIndex, memoryRequirements.size, memoryRequirements.alignment);
		Memory memory = m_Allocator.AllocateDedicated(memoryTypeIndex, memoryRequirements.size);
		ASSERT(memory.IsValid());

		vkBindBufferMemory(m_Device, buffer, memory.memory, memory.offset);

		// todo：BufferView
		// 用于computer buffer

		return new BufferResource();
	}
}