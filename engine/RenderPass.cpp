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

RenderPassKey RenderPass::GetKey()
{
	RenderPassKey key;

	key.attachments.resize(m_Attachments.size());
	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		key.attachments[i].typeMask = m_Attachments[i]->GetTypeMask();
		key.attachments[i].format = m_Attachments[i]->GetFormat();
		key.attachments[i].loadOp = m_Attachments[i]->GetLoadOp();
		key.attachments[i].storeOp = m_Attachments[i]->GetStoreOp();
	}

	key.subpasses.resize(m_Subpasses.size());
	for (size_t i = 0; i < m_Subpasses.size(); i++)
	{
		key.subpasses[i].inputs = m_Subpasses[i].inputs;
		key.subpasses[i].colors = m_Subpasses[i].colors;
		key.subpasses[i].depth = m_Subpasses[i].depth;
	}

	return key;
}
