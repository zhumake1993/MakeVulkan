#include "RenderPass.h"
#include "Tools.h"
#include "Texture.h"

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

void RenderPass::SetAttachments(const mkVector<Attachment*> actualAttachments)
{
	auto& attachments = m_RenderPassKey.GetAttachments();

	ASSERT(attachments.size() == actualAttachments.size());
	m_Images.resize(attachments.size());

	for (int i = 0; i < actualAttachments.size(); i++)
	{
		ASSERT(actualAttachments[i]->GetFormat() == attachments[i].format);
		ASSERT(actualAttachments[i]->GetWidth() == m_RenderPassKey.GetWidth());
		ASSERT(actualAttachments[i]->GetHeight() == m_RenderPassKey.GetHeight());

		m_Images[i] = actualAttachments[i]->GetImage();
	}
}