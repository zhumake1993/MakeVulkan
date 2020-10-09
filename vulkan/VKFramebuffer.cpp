#include "VKFramebuffer.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"
#include "VKRenderPass.h"

VKFramebuffer::VKFramebuffer(VKDevice * vkDevice, VKRenderPass * vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height) :
	device(vkDevice->device)
{
	VkImageView attachments[2];
	attachments[0] = color;
	attachments[1] = depth;

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.flags = 0;
	frameBufferCreateInfo.renderPass = vkRenderPass->renderPass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = attachments;
	frameBufferCreateInfo.width = width;
	frameBufferCreateInfo.height = height;
	frameBufferCreateInfo.layers = 1;

	VK_CHECK_RESULT(vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &framebuffer));
}

VKFramebuffer::~VKFramebuffer()
{
	if (device != VK_NULL_HANDLE && framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
		framebuffer = VK_NULL_HANDLE;
	}
}
