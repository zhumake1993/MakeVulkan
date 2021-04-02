#include "RenderPass.h"

Attachment::Attachment(int typeMask, VkFormat format, uint32_t width, uint32_t height, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp)
	: m_TypeMask(typeMask)
	, m_Format(format)
	, m_Width(width)
	, m_Height(height)
	, m_LoadOp(loadOp)
	, m_StoreOp(storeOp)
{
}

Attachment::~Attachment()
{
}

RenderPass::RenderPass(uint32_t width, uint32_t height)
	: m_Width(width)
	, m_Height(height)
{
}

RenderPass::~RenderPass()
{
}

uint32_t RenderPass::GetWidth()
{
	return m_Width;
}

uint32_t RenderPass::GetHeight()
{
	return m_Height;
}

void RenderPass::SetAttachments(const std::vector<Attachment*> attachments)
{
	m_Attachments = attachments;
}

void RenderPass::AddSubpass(const std::vector<int> inputs, const std::vector<int> colors, int depth)
{
	m_Subpasses.emplace_back();
	Subpass& subpass = m_Subpasses.back();
	subpass.inputs = inputs;
	subpass.colors = colors;
	subpass.depth = depth;
}

std::vector<Attachment*>& RenderPass::GetAttachments()
{
	return m_Attachments;
}
