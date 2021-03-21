#include "RenderPassManager.h"
#include "VulkanTools.h"

RenderPassManager::RenderPassManager(VkDevice vkDevice)
	: m_Device(vkDevice)
{
}

RenderPassManager::~RenderPassManager()
{
}

VKAttachment * RenderPassManager::CreateAttachment(uint32_t width, uint32_t height, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp)
{
	return nullptr;
}
