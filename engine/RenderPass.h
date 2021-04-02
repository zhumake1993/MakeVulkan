#pragma once

#include "Env.h"

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

	Attachment(int typeMask, VkFormat format, uint32_t width, uint32_t height, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp);
	virtual ~Attachment();

	int GetTypeMask() { return m_TypeMask; }
	VkFormat GetFormat() { return m_Format; }
	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeigh() { return m_Height; }
	VkAttachmentLoadOp GetLoadOp() { return m_LoadOp; }
	VkAttachmentStoreOp GetStoreOp() { return m_StoreOp; }

protected:

	int m_TypeMask;
	VkFormat m_Format;
	uint32_t m_Width;
	uint32_t m_Height;
	VkAttachmentLoadOp m_LoadOp;
	VkAttachmentStoreOp m_StoreOp;
};

struct RenderPassKey
{
	struct AttachmentKey
	{
		bool operator==(const AttachmentKey & other) const
		{
			return typeMask == other.typeMask && format == other.format && loadOp == other.loadOp && storeOp == other.storeOp;
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
			return inputs == other.inputs && colors == other.colors && depth == other.depth;
		}
		std::vector<int> inputs;
		std::vector<int> colors;
		int depth;
	};
	bool operator==(const RenderPassKey & other) const
	{
		return attachments == other.attachments && subpasses == other.subpasses;
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
			hash ^= std::hash<int>()(a.typeMask) ^ std::hash<VkFormat>()(a.format) ^ std::hash<VkAttachmentLoadOp>()(a.loadOp) ^ std::hash<VkAttachmentStoreOp>()(a.storeOp);
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

class RenderPass
{
	struct Subpass
	{
		std::vector<int> inputs;
		std::vector<int> colors;
		int depth;
	};

public:

	RenderPass(uint32_t width, uint32_t height);
	virtual ~RenderPass();

	uint32_t GetWidth();
	uint32_t GetHeight();

	void SetAttachments(const std::vector<Attachment*> attachments);
	void AddSubpass(const std::vector<int> inputs, const std::vector<int> colors, int depth = -1); // -1表示不用depth，暂时不考虑preserve

	std::vector<Attachment*>& GetAttachments();

	RenderPassKey GetKey();

protected:

	uint32_t m_Width;
	uint32_t m_Height;

	std::vector<Attachment*> m_Attachments;
	std::vector<Subpass> m_Subpasses;
};