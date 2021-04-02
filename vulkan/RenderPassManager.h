#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "RenderPass.h"

class VKImage;

class AttachmentVulkan : public Attachment
{
public:

	AttachmentVulkan(int typeMask, VkFormat format, uint32_t width, uint32_t height, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp);
	virtual ~AttachmentVulkan();

public:

	VKImage* m_Image;
};

class VKRenderPass : public VKResource
{
public:

	VKRenderPass(VkDevice vkDevice)
		: device(vkDevice)
	{
	}

	virtual ~VKRenderPass()
	{
		vkDestroyRenderPass(device, renderPass, nullptr);
	}

	VkRenderPass renderPass = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};

class VKFramebuffer : public VKResource
{
public:

	VKFramebuffer(VkDevice vkDevice)
		: device(vkDevice)
	{
	}

	virtual ~VKFramebuffer()
	{
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	VkFramebuffer framebuffer = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};

class RenderPassVulkan : public RenderPass
{
public:

	RenderPassVulkan(uint32_t width, uint32_t height)
		: RenderPass(width, height)
	{
	}

	virtual ~RenderPassVulkan()
	{
	}

	VkImageView GetInputAttachmentImageView(uint32_t inputIndex);

public:

	uint32_t m_SubpassIndex = 0;

	VKRenderPass* m_RenderPass = nullptr;
};

class RenderPassManager : public NonCopyable
{
public:

	RenderPassManager(VkDevice vkDevice);
	virtual ~RenderPassManager();

	VKRenderPass* GetRenderPass(RenderPassKey& key);

	void ReleaseRenderPass(RenderPassKey& key, VKRenderPass* renderPass);

private:

	VKRenderPass* CreateRenderPass(RenderPassKey& key);

private:

	ResourcePool<RenderPassKey, VKRenderPass, RenderPassKeyHash> m_RenderPassPool;

	VkDevice m_Device = VK_NULL_HANDLE;
};