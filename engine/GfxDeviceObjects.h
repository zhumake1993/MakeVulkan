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

// Renderpass

class RenderPassKey
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

public:

	RenderPassKey(uint32_t attachmentNum, uint32_t subpasseNum, uint32_t width, uint32_t height)
		: m_Attachments(attachmentNum)
		, m_Subpasses(subpasseNum)
		, m_Width(width)
		, m_Height(height)
	{
	}

	void SetAttachment(uint32_t index, int typeMask, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp)
	{
		m_Attachments[index].typeMask = typeMask;
		m_Attachments[index].format = format;
		m_Attachments[index].loadOp = loadOp;
		m_Attachments[index].storeOp = storeOp;
	}

	void SetSubpass(uint32_t index, const std::vector<int> inputs, const std::vector<int> colors, int depth = -1)  // -1表示不用depth，暂时不考虑preserve
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
			hash ^= std::hash<int>()(a.typeMask)
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