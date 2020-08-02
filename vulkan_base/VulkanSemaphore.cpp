#include "VulkanSemaphore.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanSemaphore::VulkanSemaphore(VulkanDevice * vulkanDevice):
	m_VulkanDevice(vulkanDevice)
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	VK_CHECK_RESULT(vkCreateSemaphore(m_VulkanDevice->m_LogicalDevice, &semaphoreCreateInfo, nullptr, &m_Semaphore));
}

VulkanSemaphore::~VulkanSemaphore()
{
}

void VulkanSemaphore::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_Semaphore != VK_NULL_HANDLE) {
		vkDestroySemaphore(m_VulkanDevice->m_LogicalDevice, m_Semaphore, nullptr);
		m_Semaphore = VK_NULL_HANDLE;
	}
}
