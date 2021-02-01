#pragma once

#include "Env.h"
#include "Log.h"

struct Color
{
	Color() :r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
	Color(float p1, float p2, float p3, float p4) :r(p1), g(p2), b(p3), a(p4) {}
	float r, g, b, a;
};

struct DepthStencil
{
	DepthStencil() :depth(1.0f), stencil(0) {}
	DepthStencil(float p1, uint32_t p2) :depth(p1), stencil(p2) {}
	float depth;
	uint32_t stencil;
};

struct Rect2D
{
	Rect2D(int32_t p1, int32_t p2, uint32_t p3, uint32_t p4) :x(p1), y(p2), width(p3), height(p4) {}
	int32_t x, y;
	uint32_t width, height;
};

struct Viewport
{
	Viewport() :x(0.0f), y(0.0f), width(0), height(0), minDepth(0.0f), maxDepth(1.0f) {}
	Viewport(uint32_t p1, uint32_t p2) :x(0.0f), y(0.0f), width(p1), height(p2), minDepth(0.0f), maxDepth(1.0f) {}
	float    x;
	float    y;
	uint32_t width;
	uint32_t height;
	float    minDepth;
	float    maxDepth;
};

enum VertexChannel
{
	kVertexPosition,
	kVertexNormal,
	kVertexColor,
	kVertexTexcoord0,

	kVertexChannelCount
};

VkFormat VertexChannelToFormat(VertexChannel channel);

uint32_t VkFormatToSize(VkFormat format);

struct VertexDescription
{
	std::vector<VkFormat> formats;
	std::vector<uint32_t> offsets;
	uint32_t stride;
};

enum BufferType
{
	kBufferTypeVertex,
	kBufferTypeIndex,
	//kBufferTypeUniform,
	kBufferTypeStaging
};

enum ImageType
{
	kImageType1D,
	kImageType2D,
	kImageType3D
};

enum UniformDataType
{
	kUniformDataTypeFloat1,
	kUniformDataTypeFloat2,
	kUniformDataTypeFloat3,
	kUniformDataTypeFloat4,

	kUniformDataTypeFloat4x4
};

std::string UniformDataTypeToString(UniformDataType type);

uint32_t UniformDataTypeToSize(UniformDataType type);

struct UniformBufferElement
{
	UniformBufferElement(UniformDataType t, std::string n) :type(t), name(n) {}

	UniformDataType type;
	std::string name;
	uint32_t offset;
};

struct UniformBufferLayout
{
	UniformBufferLayout(std::string n, int b, VkShaderStageFlags flags) :name(n), binding(b), stageFlags(flags) {}

	void Add(UniformBufferElement& element)
	{
		element.offset = GetSize();
		elements.push_back(element);
	}

	uint32_t GetSize()
	{
		if (elements.empty())
		{
			return 0;
		}
		else
		{
			return elements.back().offset + UniformDataTypeToSize(elements.back().type);
		}
	}

	std::string name;
	int binding;
	VkShaderStageFlags stageFlags;
	std::vector<UniformBufferElement> elements;
};

struct GpuParameters
{
	std::vector<UniformBufferLayout> uniformBufferLayouts;
};

struct RenderStatus
{
	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT , VK_DYNAMIC_STATE_SCISSOR };
};

enum VKShaderType
{
	kVKShaderTypeVertex,
	kVKShaderTypeFragment,
	kVKShaderTypeCount
};
