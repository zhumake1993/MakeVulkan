#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "Tools.h"

VulkanCommandPool::VulkanCommandPool(VulkanDevice* vulkanDevice, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex) :
	m_VulkanDevice(vulkanDevice),
	m_Flags(flags),
	m_QueueFamilyIndex(queueFamilyIndex)
{
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = nullptr;
	cmdPoolInfo.flags = flags;
	cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;

	VK_CHECK_RESULT(vkCreateCommandPool(vulkanDevice->m_LogicalDevice, &cmdPoolInfo, nullptr, &m_CommandPool));
}

VulkanCommandPool::~VulkanCommandPool()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_CommandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(m_VulkanDevice->m_LogicalDevice, m_CommandPool, nullptr);
		m_CommandPool = VK_NULL_HANDLE;
	}
}

VulkanCommandBuffer * VulkanCommandPool::AllocateCommandBuffer(VkCommandBufferLevel level)
{
	VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.pNext = nullptr;
	cmdBufferAllocInfo.commandPool = m_CommandPool;
	cmdBufferAllocInfo.level = level;
	cmdBufferAllocInfo.commandBufferCount = 1;

	VulkanCommandBuffer* vulkanCommandBuffer = new VulkanCommandBuffer(m_VulkanDevice, this, level);
	VK_CHECK_RESULT(vkAllocateCommandBuffers(m_VulkanDevice->m_LogicalDevice, &cmdBufferAllocInfo, &vulkanCommandBuffer->m_CommandBuffer));
	return vulkanCommandBuffer;
}
