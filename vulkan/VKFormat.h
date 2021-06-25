#pragma once

#include "VKIncludes.h"
#include "GfxTypes.h"

namespace vk
{
	VkFormat ImageFormatToVkFormat(ImageFormat format);

	ImageFormat VkFormatToImageFormat(VkFormat format);
}