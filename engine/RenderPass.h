#pragma once

#include "Env.h"

class Attachment
{
public:

	Attachment(uint32_t width, uint32_t height, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp);
	virtual ~Attachment();

protected:

	uint32_t m_Width;
	uint32_t m_Height;
	VkFormat m_Format;
	VkAttachmentLoadOp m_LoadOp;
	VkAttachmentStoreOp m_StoreOp;
};

class RenderPass
{
public:

	RenderPass();
	virtual ~RenderPass();

	static Attachment* CreateAttachment(uint32_t width, uint32_t height, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp);
};