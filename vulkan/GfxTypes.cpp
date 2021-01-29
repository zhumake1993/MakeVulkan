#include "GfxTypes.h"
#include "Tools.h"

VkFormat VertexChannelToFormat(VertexChannel channel)
{
	// 要与VertexChannel保持一致
	static std::vector<VkFormat> vertexChannelFormats = {
		VK_FORMAT_R32G32B32_SFLOAT, // kVertexPosition
		VK_FORMAT_R32G32B32_SFLOAT, // kVertexNormal
		VK_FORMAT_R32G32B32_SFLOAT, // kVertexColor
		VK_FORMAT_R32G32_SFLOAT // kVertexTexcoord
	};

	return vertexChannelFormats[channel];
}

uint32_t VkFormatToSize(VkFormat format)
{
	switch (format)
	{
	case VK_FORMAT_R32G32B32_SFLOAT:
		return 3 * 4;
		break;
	case VK_FORMAT_R32G32_SFLOAT:
		return 2 * 4;
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
		return 4 * 1;
		break;
	default:
		LOG("wrong VkFormat");
		assert(false);
		return 0;
	}
}

std::string UniformDataTypeToString(UniformDataType type)
{
	switch (type)
	{
	case kUniformDataTypeFloat1:
		return "kUniformDataTypeFloat1";
		break;
	case kUniformDataTypeFloat2:
		return "kUniformDataTypeFloat2";
		break;
	case kUniformDataTypeFloat3:
		return "kUniformDataTypeFloat3";
		break;
	case kUniformDataTypeFloat4:
		return "kUniformDataTypeFloat4";
		break;
	case kUniformDataTypeFloat4x4:
		return "kUniformDataTypeFloat4x4";
		break;
	default:
		LOG("wrong UniformDataType");
		EXIT;
		return 0;
	}
}

uint32_t UniformDataTypeToSize(UniformDataType type)
{
	switch (type)
	{
	case kUniformDataTypeFloat1:
		return 1 * 4;
		break;
	case kUniformDataTypeFloat2:
		return 2 * 4;
		break;
	case kUniformDataTypeFloat3:
		return 3 * 4;
		break;
	case kUniformDataTypeFloat4:
		return 4 * 4;
		break;
	case kUniformDataTypeFloat4x4:
		return 4 * 4 * 4;
		break;
	default:
		LOG("wrong UniformDataType");
		EXIT;
		return 0;
	}
}