#include "RenderPass.h"

Attachment::Attachment(int typeMask, VkFormat format, uint32_t width, uint32_t height)
	: m_TypeMask(typeMask)
	, m_Format(format)
	, m_Width(width)
	, m_Height(height)
{
}

Attachment::~Attachment()
{
}

RenderPass::RenderPass(RenderPassKey& renderPassKey)
	: m_RenderPassKey(renderPassKey)
{
}

RenderPass::~RenderPass()
{
}

RenderPassKey & RenderPass::GetKey()
{
	return m_RenderPassKey;
}

uint32_t RenderPass::GetWidth()
{
	return m_RenderPassKey.GetWidth();
}

uint32_t RenderPass::GetHeight()
{
	return m_RenderPassKey.GetHeight();
}

void RenderPass::SetAttachments(const std::vector<Attachment*> attachments)
{
	m_Attachments = attachments;
}

std::vector<Attachment*>& RenderPass::GetAttachments()
{
	return m_Attachments;
}
