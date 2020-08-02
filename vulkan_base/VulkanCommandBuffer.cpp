#include "VulkanCommandBuffer.h"

VulkanCommandBuffer::VulkanCommandBuffer()
{
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

VkCommandBuffer& VulkanCommandBuffer::operator*()
{
	return m_CommandBuffer;
}
