#include "VKTypes.h"
#include "Tools.h"

uint32_t VkFormatToSize(VkFormat format) {
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



uint32_t UniformTypeToSize(UniformType type)
{
	switch (type)
	{
	case float1:
		return 1 * 4;
		break;
	case float2:
		return 2 * 4;
		break;
	case float3:
		return 3 * 4;
		break;
	case float4:
		return 4 * 4;
		break;
	case float4x4:
		return 4 * 4 * 4;
		break;
	default:
		LOG("wrong UniformType");
		assert(false);
		return 0;
	}
}
