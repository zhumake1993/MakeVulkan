#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* vulkanDevice, VulkanCommandPool* vulkanCommandPool, VkCommandBufferLevel level):
	m_VulkanDevice(vulkanDevice),
	m_VulkanCommandPool(vulkanCommandPool),
	m_Level(level)
{
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

void VulkanCommandBuffer::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE
		&& m_VulkanCommandPool && m_VulkanCommandPool->m_CommandPool != VK_NULL_HANDLE
		&& m_CommandBuffer != VK_NULL_HANDLE) {
		vkFreeCommandBuffers(m_VulkanDevice->m_LogicalDevice, m_VulkanCommandPool->m_CommandPool, 1, &m_CommandBuffer);
		m_CommandBuffer = VK_NULL_HANDLE;
	}
}