#pragma once

#include "Env.h"
#include "GfxDeviceObjects.h"

enum AttachmentTypeMask
{
	kAttachmentColor		= (1 << 0),
	kAttachmentDepth		= (1 << 1),
	kAttachmentInput		= (1 << 2),
	kAttachmentSwapChain	= (1 << 3)
};

class Attachment
{
public:

	Attachment(int typeMask, VkFormat format, uint32_t width, uint32_t height);
	virtual ~Attachment();

	int GetTypeMask() { return m_TypeMask; }
	VkFormat GetFormat() { return m_Format; }
	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeigh() { return m_Height; }

protected:

	int m_TypeMask;
	VkFormat m_Format;
	uint32_t m_Width;
	uint32_t m_Height;
};

class RenderPass
{
	struct Subpass
	{
		std::vector<int> inputs;
		std::vector<int> colors;
		int depth;
	};

public:

	RenderPass(RenderPassKey& renderPassKey);
	virtual ~RenderPass();

	RenderPassKey& GetKey();
	uint32_t GetWidth();
	uint32_t GetHeight();

	void SetAttachments(const std::vector<Attachment*> attachments);
	std::vector<Attachment*>& GetAttachments();

protected:

	RenderPassKey m_RenderPassKey;

	std::vector<Attachment*> m_Attachments;
};