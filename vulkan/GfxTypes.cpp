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

uint32_t ShaderDataTypeToSize(ShaderDataType type)
{
	switch (type)
	{
	case kShaderDataFloat1:
		return 1 * 4;
		break;
	case kShaderDataFloat2:
		return 2 * 4;
		break;
	case kShaderDataFloat3:
		return 3 * 4;
		break;
	case kShaderDataFloat4:
		return 4 * 4;
		break;
	case kShaderDataFloat4x4:
		return 4 * 4 * 4;
		break;
	case kShaderDataInt1:
		return 1 * 4;
		break;
	default:
		LOG("wrong UniformDataType");
		EXIT;
		return 0;
	}
}