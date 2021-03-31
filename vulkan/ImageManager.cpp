#include "ImageManager.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"
#include "GfxDevice.h"

ImageManager::ImageManager(VkDevice vkDevice)
	: m_Device(vkDevice)
{
}

ImageManager::~ImageManager()
{
}

VKImage * ImageManager::GetImage(VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount, VkImageUsageFlags usage)
{
	size_t hash
		= std::hash<VkImageType>()(vkImageType)
		^ std::hash<VkFormat>()(format)
		^ std::hash<uint32_t>()(width)
		^ std::hash<uint32_t>()(height)
		^ std::hash<uint32_t>()(mipLevels)
		^ std::hash<uint32_t>()(layerCount)
		^ std::hash<uint32_t>()(faceCount)
		^ std::hash<VkImageUsageFlags>()(usage);

	VKImage* image = m_ImagePool.Get(hash);
	if (image)
	{
		return image;
	}
	else
	{
		VKImage* image = CreateImage(vkImageType, format, width, height, mipLevels, layerCount, faceCount, usage);
		return image;
	}
}

VKImageView * ImageManager::GetImageView(VkImage image, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount)
{
	//todo
	return CreateImageView(image, vkImageViewType, vkFormat, vkAspectMask, mipLevels, layerCount, faceCount);
}

VKImageSampler * ImageManager::GetImageSampler(uint32_t mipLevels, float maxAnisotropy)
{
	//todo
	return CreateImageSampler(mipLevels, maxAnisotropy);
}

void ImageManager::ReleaseImage(VKImage* image)
{
	m_ImagePool.Add(image);
}

//VKImageView * ImageManager::ReleaseImageView(VkImage image, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount)
//{
//	return nullptr;
//}

VKImage * ImageManager::CreateImage(VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount, VkImageUsageFlags usage)
{
	VKImage* image = new VKImage(m_Device);

	image->imageType = vkImageType;
	image->format = format;
	image->width = width;
	image->height = height;
	image->mipLevels = mipLevels;
	image->layerCount = layerCount;
	image->faceCount = faceCount;
	image->usage = usage;

	// Image

	VkImageCreateInfo imageCI = {};
	imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCI.pNext = nullptr;
	imageCI.flags = faceCount > 1 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCI.imageType = vkImageType;
	imageCI.format = format;
	imageCI.extent.width = width;
	imageCI.extent.height = height;
	imageCI.extent.depth = 1;
	imageCI.mipLevels = mipLevels;
	imageCI.arrayLayers = faceCount * layerCount; // Cube faces count as array layers in Vulkan
	imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCI.usage = usage;
	imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCI.queueFamilyIndexCount = 0;
	imageCI.pQueueFamilyIndices = nullptr;
	imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VK_CHECK_RESULT(vkCreateImage(m_Device, &imageCI, nullptr, &image->image));

	// Memory

	auto& dp = GetDeviceProperties();

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(m_Device, image->image, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = dp.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &image->memory));

	// Bind

	VK_CHECK_RESULT(vkBindImageMemory(m_Device, image->image, image->memory, 0));

	return image;
}

VKImageView * ImageManager::CreateImageView(VkImage image, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount)
{
	VKImageView* view = new VKImageView(m_Device);

	view->imageViewType = vkImageViewType;
	view->vkAspectMask = vkAspectMask;

	VkImageViewCreateInfo imageViewCI = {};
	imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCI.pNext = nullptr;
	imageViewCI.flags = 0;
	imageViewCI.image = image;
	imageViewCI.viewType = vkImageViewType;
	imageViewCI.format = vkFormat;
	imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.subresourceRange.aspectMask = vkAspectMask;
	imageViewCI.subresourceRange.baseMipLevel = 0;
	imageViewCI.subresourceRange.levelCount = mipLevels;
	imageViewCI.subresourceRange.baseArrayLayer = 0;
	imageViewCI.subresourceRange.layerCount = faceCount * layerCount;

	VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageViewCI, nullptr, &view->view));

	return view;
}

VKImageSampler * ImageManager::CreateImageSampler(uint32_t mipLevels, float maxAnisotropy)
{
	VKImageSampler* sampler = new VKImageSampler(m_Device);

	sampler->maxAnisotropy = maxAnisotropy;

	VkSamplerCreateInfo samplerCI = {};
	samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCI.pNext = nullptr;
	samplerCI.flags = 0;
	samplerCI.magFilter = VK_FILTER_LINEAR;
	samplerCI.minFilter = VK_FILTER_LINEAR;
	samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	samplerCI.mipLodBias = 0.0f;

	auto& dp = GetDeviceProperties();
	if (dp.enabledDeviceFeatures.samplerAnisotropy)
	{
		if (maxAnisotropy > dp.deviceProperties.limits.maxSamplerAnisotropy)
		{
			maxAnisotropy = dp.deviceProperties.limits.maxSamplerAnisotropy;
		}
		samplerCI.anisotropyEnable = VK_TRUE;
		samplerCI.maxAnisotropy = maxAnisotropy;
	}
	else
	{
		samplerCI.anisotropyEnable = VK_FALSE;
		samplerCI.maxAnisotropy = 1.0f;
	}

	samplerCI.compareEnable = VK_FALSE;
	samplerCI.compareOp = VK_COMPARE_OP_NEVER;
	samplerCI.minLod = 0.0f;
	samplerCI.maxLod = static_cast<float>(mipLevels);
	samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCI.unnormalizedCoordinates = VK_FALSE;

	VK_CHECK_RESULT(vkCreateSampler(m_Device, &samplerCI, nullptr, &sampler->sampler));

	return sampler;
}
