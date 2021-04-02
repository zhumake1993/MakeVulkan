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