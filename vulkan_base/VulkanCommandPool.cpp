#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

VulkanCommandPool::VulkanCommandPool(VulkanDevice* vulkanDevice) :m_VulkanDevice(vulkanDevice)
{
}

VulkanCommandPool::~VulkanCommandPool()
{
}

void VulkanCommandPool::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_CommandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(m_VulkanDevice->m_LogicalDevice, m_CommandPool, nullptr);
	}
}