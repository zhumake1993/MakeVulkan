#include "VulkanFramebuffer.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice * vulkanDevice):
	m_VulkanDevice(vulkanDevice)
{
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_Framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(m_VulkanDevice->m_LogicalDevice, m_Framebuffer, nullptr);
		m_Framebuffer = VK_NULL_HANDLE;
	}
}