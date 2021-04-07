#pragma once

#include "Env.h"
#include "GfxTypes.h"

struct MaterialBindData
{
	struct UniformBufferBinding
	{
		UniformBufferBinding(int p1, void* p2) :binding(p1), buffer(p2) {}
		int binding;
		void* buffer;
	};

	struct ImageBinding
	{
		ImageBinding(int p1, void* p2) :binding(p1), image(p2) {}
		int binding;
		void* image;
	};

	struct InputAttachmentBinding
	{
		InputAttachmentBinding(int _binding) :binding(_binding) {}
		int binding;
	};

	std::vector<UniformBufferBinding> uniformBufferBindings;
	std::vector<ImageBinding> imageBindings;
	std::vector<InputAttachmentBinding> inputAttachmentBindings;
};

struct DrawBuffer
{
	void* vertexBuffer; // Buffer
	void* indexBuffer; // Buffer
	void* vertexDescription; // VertexDescription
	VkIndexType indexType;
	uint32_t indexCount;
};

struct DrawItem
{
	size_t drawBufferIndex;
};

struct DrawBatchs
{
	void* gpuProgram;

	size_t alignedUniformSize;

	// uniform
	int uniformBinding;
	void* uniformData;
	uint64_t uniformSize;

	std::vector<DrawBuffer> drawBuffers;
	std::vector<DrawItem> drawItems;
};

// Image

enum ImageTypeMask
{
	// Swapchain�е�image
	kImageSwapChainBit			= (1 << 0),

	// Ӱ��VkImageAspectFlags
	kImageColorAspectBit		= (1 << 1),
	kImageDepthAspectBit		= (1 << 2),

	// Ӱ��VkImageUsageFlags
	kImageTransferSrcBit		= (1 << 3),
	kImageTransferDstBit		= (1 << 4),
	kImageSampleBit				= (1 << 5), // ��shader�б�����
	kImageColorAttachmentBit	= (1 << 6), // ��renderpass����ΪcolorAttachment 
	kImageDepthAttachmentBit	= (1 << 7), // ��renderpass����ΪdepthAttachment 
	kImageInputAttachmentBit	= (1 << 8), // ��renderpass����ΪinputAttachment 
};

// Attachment

enum AttachmentType
{
	kAttachmentSwapChain		= (1 << 0),
	kAttachmentColor			= (1 << 1),
	kAttachmentDepth			= (1 << 2),
	kAttachmentSample			= (1 << 3),
};

// Renderpass

class RenderPassKey
{
	struct AttachmentKey
	{
		bool operator==(const AttachmentKey & other) const
		{
			return attachmentType == other.attachmentType
				&& format == other.format
				&& loadOp == other.loadOp
				&& storeOp == other.storeOp;
		}
		AttachmentType attachmentType;
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

public:

	RenderPassKey(uint32_t attachmentNum, uint32_t subpasseNum, uint32_t width, uint32_t height)
		: m_Attachments(attachmentNum)
		, m_Subpasses(subpasseNum)
		, m_Width(width)
		, m_Height(height)
	{
	}

	void SetAttachment(uint32_t index, AttachmentType attachmentType, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp)
	{
		m_Attachments[index].attachmentType = attachmentType;
		m_Attachments[index].format = format;
		m_Attachments[index].loadOp = loadOp;
		m_Attachments[index].storeOp = storeOp;
	}

	void SetSubpass(uint32_t index, const std::vector<int> inputs, const std::vector<int> colors, int depth = -1)  // -1��ʾ����depth����ʱ������preserve
	{
		m_Subpasses[index].inputs = inputs;
		m_Subpasses[index].colors = colors;
		m_Subpasses[index].depth = depth;
	}

	const std::vector<AttachmentKey>& GetAttachments() const
	{
		return m_Attachments;
	}

	const std::vector<SubpassKey>& GetSubpasses() const
	{
		return m_Subpasses;
	}

	uint32_t GetWidth()
	{
		return m_Width;
	}

	uint32_t GetHeight()
	{
		return m_Height;
	}

	bool operator==(const RenderPassKey & other) const
	{
		return m_Attachments == other.m_Attachments
			&& m_Subpasses == other.m_Subpasses;
	}

private:

	std::vector<AttachmentKey> m_Attachments;
	std::vector<SubpassKey> m_Subpasses;
	uint32_t m_Width;
	uint32_t m_Height;
};

struct RenderPassKeyHash
{
	size_t operator()(const RenderPassKey & renderPassKey) const
	{
		size_t hash = 0;
		for (auto& a : renderPassKey.GetAttachments())
		{
			hash ^= std::hash<int>()(a.attachmentType)
				^ std::hash<int>()(a.format)
				^ std::hash<int>()(a.loadOp)
				^ std::hash<int>()(a.storeOp);
		}
		for (auto& s : renderPassKey.GetSubpasses())
		{
			hash ^= std::hash<int>()(s.depth);
			for (auto i : s.inputs) hash ^= std::hash<int>()(i);
			for (auto c : s.colors) hash ^= std::hash<int>()(c);
		}
		return hash;
	}
};