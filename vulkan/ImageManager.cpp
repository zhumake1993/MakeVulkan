#include "ImageManager.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"
#include "GarbageCollector.h"
#include "GfxDevice.h"

ImageImpl::ImageImpl(VKImage * image, VKImageView * view, VKImageSampler * sampler) :
	m_Image(image), m_View(view), m_Sampler(sampler)
{
}

ImageImpl::~ImageImpl()
{
	GetGfxDevice().ReleaseImage(this);
}

ImageManager::ImageManager(VkDevice vkDevice, GarbageCollector * gc) :
	m_Device(vkDevice),
	m_GarbageCollector(gc)
{
}

ImageManager::~ImageManager()
{
}

VKImage * ImageManager::CreateImage(VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, VkImageUsageFlags usage)
{
	VKImage* image = new VKImage(m_Device, vkImageType, format, width, height, mipLevels, layerCount, usage);

	// Image

	VkImageCreateInfo imageCI = {};
	imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCI.pNext = nullptr;
	imageCI.flags = 0;
	imageCI.imageType = vkImageType;
	imageCI.format = format;
	imageCI.extent.width = width;
	imageCI.extent.height = height;
	imageCI.extent.depth = 1;
	imageCI.mipLevels = mipLevels;
	imageCI.arrayLayers = layerCount;
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

VKImageView * ImageManager::CreateView(VkImage image, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask, uint32_t mipLevels, uint32_t layerCount)
{
	VKImageView* view = new VKImageView(m_Device, image, vkImageViewType, vkFormat, vkAspectMask);

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
	imageViewCI.subresourceRange.layerCount = layerCount;

	VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageViewCI, nullptr, &view->view));

	return view;
}

VKImageSampler * ImageManager::CreateSampler(uint32_t mipLevels, float maxAnisotropy)
{
	VKImageSampler* sampler = new VKImageSampler(m_Device);

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