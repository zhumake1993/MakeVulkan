#include "VulkanFence.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanFence::VulkanFence(VulkanDevice * vulkanDevice, bool signaled):
	m_VulkanDevice(vulkanDevice),
	m_Signaled(signaled)
{
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VK_CHECK_RESULT(vkCreateFence(m_VulkanDevice->m_LogicalDevice, &fenceCreateInfo, nullptr, &m_Fence));
}

VulkanFence::~VulkanFence()
{
}

void VulkanFence::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_Fence != VK_NULL_HANDLE) {
		vkDestroyFence(m_VulkanDevice->m_LogicalDevice, m_Fence, nullptr);
		m_Fence = VK_NULL_HANDLE;
	}
}

void VulkanFence::Wait()
{
	VK_CHECK_RESULT(vkWaitForFences(m_VulkanDevice->m_LogicalDevice, 1, &m_Fence, VK_TRUE, UINT64_MAX));
}

void VulkanFence::Reset()
{
	VK_CHECK_RESULT(vkResetFences(m_VulkanDevice->m_LogicalDevice, 1, &m_Fence));
}
