#include "VulkanFramebuffer.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "Tools.h"

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* vulkanDevice, VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height):
	m_VulkanDevice(vulkanDevice)
{
	VkImageView attachments[2];
	attachments[0] = color;
	attachments[1] = depth;

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.flags = 0;
	frameBufferCreateInfo.renderPass = vulkanRenderPass->m_RenderPass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = attachments;
	frameBufferCreateInfo.width = width;
	frameBufferCreateInfo.height = height;
	frameBufferCreateInfo.layers = 1;

	VK_CHECK_RESULT(vkCreateFramebuffer(m_VulkanDevice->m_LogicalDevice, &frameBufferCreateInfo, nullptr, &m_Framebuffer));
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_Framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(m_VulkanDevice->m_LogicalDevice, m_Framebuffer, nullptr);
		m_Framebuffer = VK_NULL_HANDLE;
	}
}