#include "ImageManager.h"
#include "DeviceProperties.h"
#include "VKTools.h"
#include "VKMemory.h"

ImageKey ImageVulkan::GetKey()
{
	ImageKey key;
	key.imageType = m_ImageType;
	key.format = m_Format;
	key.width = m_Width;
	key.height = m_Height;
	key.mipLevels = m_MipLevels;
	key.layerCount = m_LayerCount;
	key.faceCount = m_FaceCount;
	key.usage = m_Usage;
	key.imageViewType = m_ImageViewType;
	key.aspectMask = m_AspectMask;

	return key;
}


ImageManager::ImageManager(VkDevice vkDevice)
	: m_Device(vkDevice)
{
}

ImageManager::~ImageManager()
{
	for (auto itr = m_ImageSamplerPool.begin(); itr != m_ImageSamplerPool.end(); itr++)
	{
		delete itr->second;
	}
	m_ImageSamplerPool.clear();
}

VKImage * ImageManager::GetImage(const ImageKey& key)
{
	VKImage* image = m_ImagePool.Get(key);
	if (image)
	{
		return image;
	}
	else
	{
		return CreateImage(key);
	}
}

VKImageSampler * ImageManager::GetImageSampler(const ImageSamplerKey& key)
{
	if (m_ImageSamplerPool.find(key) != m_ImageSamplerPool.end())
	{
		return m_ImageSamplerPool[key];
	}
	else
	{
		VKImageSampler* sampler = CreateImageSampler(key);
		m_ImageSamplerPool[key] = sampler;
		return sampler;
	}
}

void ImageManager::ReleaseImage(const ImageKey& key, VKImage* image)
{
	m_ImagePool.Add(key, image);
}

VKImage * ImageManager::CreateImage(const ImageKey& key)
{
	VKImage* image = new VKImage(m_Device);

	// Image

	VkImageCreateInfo imageCI = {};
	imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCI.pNext = nullptr;
	imageCI.flags = key.faceCount > 1 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCI.imageType = key.imageType;
	imageCI.format = key.format;
	imageCI.extent.width = key.width;
	imageCI.extent.height = key.height;
	imageCI.extent.depth = 1;
	imageCI.mipLevels = key.mipLevels;
	imageCI.arrayLayers = key.faceCount * key.layerCount; // Cube faces count as array layers in Vulkan
	imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCI.usage = key.usage;
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
	memoryAllocateInfo.memoryTypeIndex = vk::FindMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &image->memory));

	// Bind

	VK_CHECK_RESULT(vkBindImageMemory(m_Device, image->image, image->memory, 0));

	// ImageView

	VkImageViewCreateInfo imageViewCI = {};
	imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCI.pNext = nullptr;
	imageViewCI.flags = 0;
	imageViewCI.image = image->image;
	imageViewCI.viewType = key.imageViewType;
	imageViewCI.format = key.format;
	imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.subresourceRange.aspectMask = key.aspectMask;
	imageViewCI.subresourceRange.baseMipLevel = 0;
	imageViewCI.subresourceRange.levelCount = key.mipLevels;
	imageViewCI.subresourceRange.baseArrayLayer = 0;
	imageViewCI.subresourceRange.layerCount = key.faceCount * key.layerCount;

	VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageViewCI, nullptr, &image->view));

	return image;
}

VKImageSampler * ImageManager::CreateImageSampler(const ImageSamplerKey& key)
{
	VKImageSampler* sampler = new VKImageSampler(m_Device);

	sampler->m_MaxAnisotropy = key.maxAnisotropy;
	sampler->m_MipLevels = key.mipLevels;

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

	/*auto& dp = GetDeviceProperties();
	if (key.maxAnisotropy > 1 && dp.enabledDeviceFeatures.samplerAnisotropy)
	{
		samplerCI.anisotropyEnable = VK_TRUE;
		if (key.maxAnisotropy > dp.deviceProperties.limits.maxSamplerAnisotropy)
		{
			samplerCI.maxAnisotropy = dp.deviceProperties.limits.maxSamplerAnisotropy;
		}
		else
		{
			samplerCI.maxAnisotropy = key.maxAnisotropy;
		}
	}
	else
	{
		samplerCI.anisotropyEnable = VK_FALSE;
		samplerCI.maxAnisotropy = 1.0f;
	}*/
	samplerCI.anisotropyEnable = VK_FALSE;
	samplerCI.maxAnisotropy = 1.0f;

	samplerCI.compareEnable = VK_FALSE;
	samplerCI.compareOp = VK_COMPARE_OP_NEVER;
	samplerCI.minLod = 0.0f;
	samplerCI.maxLod = static_cast<float>(key.mipLevels);
	samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCI.unnormalizedCoordinates = VK_FALSE;

	VK_CHECK_RESULT(vkCreateSampler(m_Device, &samplerCI, nullptr, &sampler->sampler));

	return sampler;
}