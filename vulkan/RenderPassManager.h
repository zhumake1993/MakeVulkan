#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "RenderPass.h"

class VKRenderPass : public vk::VKResource
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

class VKFramebuffer : public vk::VKResource
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

	RenderPassVulkan(RenderPassKey& renderPassKey)
		: RenderPass(renderPassKey)
	{
	}

	virtual ~RenderPassVulkan()
	{
	}

	mkVector<Image*>& GetImages();

	// shaderÖÐµÄinput_attachment_index
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

	VKRenderPass* GetRenderPass(const RenderPassKey& key);

	void ReleaseRenderPass(const RenderPassKey& key, VKRenderPass* renderPass);

private:

	VKRenderPass* CreateRenderPass(const RenderPassKey& key);

private:

	ResourcePool<RenderPassKey, VKRenderPass, RenderPassKeyHash> m_RenderPassPool;

	VkDevice m_Device = VK_NULL_HANDLE;
};