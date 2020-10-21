#include "VKTypes.h"
#include "Tools.h"

VkFormat VertexChannelVkFormats[kVertexChannelCount] = {
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32_SFLOAT
};

VkFormat VertexChannelToDefaultVkFormat(int channel)
{
	assert(channel >= 0 && channel < kVertexChannelCount);
	return VertexChannelVkFormats[channel];
}

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