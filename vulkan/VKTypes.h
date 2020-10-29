#pragma once

#include "Common.h"

enum VKShaderType
{
	kVKShaderVertex = 0,
	kVKShaderFragment = 1,
	kVKShaderCount
};

enum VertexChannel {
	kVertexPosition,
	kVertexNormal,
	kVertexColor,
	kVertexTexcoord,
	//kVertexTangent,
	//kVertexBitangent,

	kVertexChannelCount
};

uint32_t VkFormatToSize(VkFormat format);

struct VertexDescription
{
	std::vector<VkFormat> formats;
	std::vector<uint32_t> offsets;
	uint32_t stride;
};

enum UniformType {
	float1,
	float2,
	float3,
	float4,

	float4x4
};

uint32_t UniformTypeToSize(UniformType type);

struct UniformElement
{
	UniformType type;
	uint32_t offset;
};