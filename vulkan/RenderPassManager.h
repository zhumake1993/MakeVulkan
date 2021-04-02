#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "RenderPass.h"
#include "ImageManager.h"

class VKImage;

class AttachmentVulkan : public Attachment
{
public:

	AttachmentVulkan(int typeMask, VkFormat format, uint32_t width, uint32_t height, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp);
	virtual ~AttachmentVulkan();

	ImageKey GetKey();

public:

	VkImageUsageFlags m_Usage;
	VkImageAspectFlags m_AspectMask;

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

struct RenderPassKey
{
	struct AttachmentKey
	{
		bool operator==(const AttachmentKey & other) const
		{
			return typeMask == other.typeMask 
				&& format == other.format 
				&& loadOp == other.loadOp 
				&& storeOp == other.storeOp;
		}
		int typeMask;
		VkFormat format;
		VkAttachmentLoadOp loadOp;
		VkAttachmentStoreOp storeOp;
	};
	struct SubpassKey
	{
		bool operator==(const SubpassKey & other) const
		{
			return inputs == other.inputs 
				&& colors == other.colors 
				&& depth == other.depth;
		}
		std::vector<int> inputs;
		std::vector<int> colors;
		int depth;
	};
	bool operator==(const RenderPassKey & other) const
	{
		return attachments == other.attachments
			&& subpasses == other.subpasses;
	}
	std::vector<AttachmentKey> attachments;
	std::vector<SubpassKey> subpasses;
};

struct RenderPassKeyHash
{
	size_t operator()(const RenderPassKey & renderPassKey) const
	{
		size_t hash = 0;
		for (auto& a : renderPassKey.attachments)
		{
			hash ^= std::hash<int>()(a.typeMask) 
				^ std::hash<int>()(a.format) 
				^ std::hash<int>()(a.loadOp)
				^ std::hash<int>()(a.storeOp);
		}
		for (auto& s : renderPassKey.subpasses)
		{
			hash ^= std::hash<int>()(s.depth);
			for (auto i : s.inputs) hash ^= std::hash<int>()(i);
			for (auto c : s.colors) hash ^= std::hash<int>()(c);
		}
		return hash;
	}
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

	RenderPassKey GetKey();

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