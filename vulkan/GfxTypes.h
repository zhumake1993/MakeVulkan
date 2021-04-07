#pragma once

#include "Env.h"
#include "Log.h"

struct Color
{
	Color(float p1, float p2, float p3, float p4) :r(p1), g(p2), b(p3), a(p4) {}
	float r, g, b, a;
};

struct DepthStencil
{
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
	Viewport(float p1, float p2, uint32_t p3, uint32_t p4, float p5, float p6) :x(p1), y(p2), width(p3), height(p4), minDepth(p5), maxDepth(p6) {}
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
	kVertexColor, // obj并不支持顶点色
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

enum BufferUsageType
{
	kBufferUsageVertex,
	kBufferUsageIndex,
	kBufferUsageUniform
};

enum MemoryPropertyType
{
	kMemoryPropertyDeviceLocal,
	kMemoryPropertyHostVisible,
	kMemoryPropertyHostCoherent
};

enum ShaderDataType
{
	kShaderDataFloat1,
	kShaderDataFloat2,
	kShaderDataFloat3,
	kShaderDataFloat4,
	kShaderDataFloat4x4,

	kShaderDataInt1
};

uint32_t ShaderDataTypeToSize(ShaderDataType type);

// 渲染状态

struct RasterizationState
{
	VkCullModeFlags          cullMode = VK_CULL_MODE_BACK_BIT;
	VkFrontFace              frontFace = VK_FRONT_FACE_CLOCKWISE;
};

struct DepthStencilState
{
	VkBool32                 depthTestEnable = VK_TRUE;
	VkBool32                 depthWriteEnable = VK_TRUE;
};

struct BlendState
{
	VkBool32                 blendEnable = VK_FALSE;
	VkBlendFactor            srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	VkBlendFactor            dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	VkBlendOp                colorBlendOp = VK_BLEND_OP_ADD;
	VkBlendFactor            srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	VkBlendFactor            dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	VkBlendOp                alphaBlendOp = VK_BLEND_OP_ADD;
	VkColorComponentFlags    colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
};

struct RenderState
{
	RenderState() : blendStates(1)
	{}

	RasterizationState rasterizationState;
	std::vector<BlendState> blendStates;
	DepthStencilState depthStencilState;
	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT , VK_DYNAMIC_STATE_SCISSOR };
};

enum VKShaderType
{
	kVKShaderTypeVertex,
	kVKShaderTypeFragment,
	kVKShaderTypeCount
};