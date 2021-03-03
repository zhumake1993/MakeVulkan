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

struct AttachmentDesc
{
	VkFormat format;
	VkAttachmentLoadOp loadOp;
	VkAttachmentStoreOp storeOp;

	size_t Hash()
	{
		return std::hash<int>()(format) ^ std::hash<int>()(loadOp) ^ std::hash<int>()(storeOp);
	}
};

struct SubPassDesc
{
	std::vector<int> inputs;
	std::vector<int> colors;
	bool useDepthStencil = false;
	std::vector<int> preserves;

	size_t Hash()
	{
		size_t hash = std::hash<bool>()(useDepthStencil);
		for (auto i : inputs)
		{
			hash ^= std::hash<int>()(i);
		}
		for (auto i : colors)
		{
			hash ^= std::hash<int>()(i);
		}
		for (auto i : preserves)
		{
			hash ^= std::hash<int>()(i);
		}
		return hash;
	}
};

struct RenderPassDesc
{
	RenderPassDesc()
	{
		memset(this, 0, sizeof(*this));
	}

	size_t Hash()
	{
		if (hash)
		{
			return hash;
		}

		hash = std::hash<int>()(present) ^ std::hash<int>()(depthStencil);
		for (auto& atta : attachmentDescs)
		{
			hash ^= atta.Hash();
		}
		for (auto& pass : subPassDescs)
		{
			hash ^= pass.Hash();
		}

		return hash;
	}

	std::vector<AttachmentDesc> attachmentDescs;
	int present = -1;
	int depthStencil = -1;

	std::vector<SubPassDesc> subPassDescs;

	size_t hash = 0;
};

struct RenderPassHash
{
	size_t operator()(RenderPassDesc & p)
	{
		return p.Hash();
	}
};