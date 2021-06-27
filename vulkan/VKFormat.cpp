#include "VKFormat.h"
#include "Log.h"

namespace vk
{
	VkFormat ImageFormatToVkFormat(ImageFormat format)
	{
		switch (format)
		{
		case kImageFormat_R8G8B8A8_UNORM:return VK_FORMAT_R8G8B8A8_UNORM;

		case kImageFormat_D32_SFLOAT_S8_UINT:return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case kImageFormat_D32_SFLOAT:return VK_FORMAT_D32_SFLOAT;
		case kImageFormat_D24_UNORM_S8_UINT:return VK_FORMAT_D24_UNORM_S8_UINT;
		case kImageFormat_D16_UNORM_S8_UINT:return VK_FORMAT_D16_UNORM_S8_UINT;
		case kImageFormat_D16_UNORM:return VK_FORMAT_D16_UNORM;

		default:LOGE("Wrong ImageFormat.");
		}
	}

	ImageFormat VkFormatToImageFormat(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_R8G8B8A8_UNORM:return kImageFormat_R8G8B8A8_UNORM;

		case VK_FORMAT_D32_SFLOAT_S8_UINT:return kImageFormat_D32_SFLOAT_S8_UINT;
		case VK_FORMAT_D32_SFLOAT:return kImageFormat_D32_SFLOAT;
		case VK_FORMAT_D24_UNORM_S8_UINT:return kImageFormat_D24_UNORM_S8_UINT;
		case VK_FORMAT_D16_UNORM_S8_UINT:return kImageFormat_D16_UNORM_S8_UINT;
		case VK_FORMAT_D16_UNORM:return kImageFormat_D16_UNORM;

		default:LOGE("Wrong VkFormat.");
		}
	}
}