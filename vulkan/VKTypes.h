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