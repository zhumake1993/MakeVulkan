#include "Transform.h"
#include "RenderPass.h"

Attachment::Attachment(uint32_t width, uint32_t height, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp)
	: m_Width(width)
	, m_Height(height)
	, m_Format(format)
	, m_LoadOp(loadOp)
	, m_StoreOp(storeOp)
{
}

Attachment::~Attachment()
{
}

Attachment * RenderPass::CreateAttachment(uint32_t width, uint32_t height, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp)
{
	return nullptr;
}
