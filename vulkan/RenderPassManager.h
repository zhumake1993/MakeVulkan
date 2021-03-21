#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "RenderPass.h"

class VKAttachment : public Attachment
{

};

class RenderPassManager : public NonCopyable
{
public:

	RenderPassManager(VkDevice vkDevice);
	virtual ~RenderPassManager();

	VKAttachment* CreateAttachment(uint32_t width, uint32_t height, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp);

private:

	/*struct AttachmentKey
	{
		uint32_t m_Width;
		uint32_t m_Height;
		VkFormat m_Format;
		VkAttachmentLoadOp m_LoadOp;
		VkAttachmentStoreOp m_StoreOp;
	};

	using AttachmenList = std::list<VKAttachment*>;
	std::unordered_map<AttachmentKey, AttachmenList> m_AttachmenCache;*/

	VkDevice m_Device = VK_NULL_HANDLE;
};