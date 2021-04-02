#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "Image.h"
#include "VKResource.h"

class VKImage : public VKResource
{
public:

	VKImage(VkDevice vkDevice)
		: device(vkDevice)
	{
	}

	virtual ~VKImage()
	{
		vkDestroyImage(device, image, nullptr);
		vkFreeMemory(device, memory, nullptr);
		vkDestroyImageView(device, view, nullptr);
	}

	size_t Hash()
	{
		if (hash)
		{
			return hash;
		}

		hash
			= std::hash<VkImageType>()(imageType)
			^ std::hash<VkFormat>()(format)
			^ std::hash<uint32_t>()(width)
			^ std::hash<uint32_t>()(height)
			^ std::hash<uint32_t>()(mipLevels)
			^ std::hash<uint32_t>()(layerCount)
			^ std::hash<uint32_t>()(faceCount)
			^ std::hash<VkImageUsageFlags>()(usage)
			^ std::hash<VkImageViewType>()(imageViewType)
			^ std::hash<VkImageAspectFlags>()(aspectMask);

		return hash;
	}

	// image
	VkImageType imageType;
	VkFormat format;
	uint32_t width;
	uint32_t height;
	uint32_t mipLevels;
	uint32_t layerCount;
	uint32_t faceCount;
	VkImageUsageFlags usage;

	// view
	// 简单起见，view的属性与image保持一致
	VkImageViewType imageViewType;
	VkImageAspectFlags aspectMask;

	size_t hash = 0;

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};

// Sampler常驻，不需要继承VKResource
class VKImageSampler
{
public:

	VKImageSampler(VkDevice vkDevice)
		: device(vkDevice)
	{
	}

	virtual ~VKImageSampler()
	{
		vkDestroySampler(device, sampler, nullptr);
	}

	size_t Hash()
	{
		if (hash)
		{
			return hash;
		}

		hash = std::hash<float>()(maxAnisotropy);

		return hash;
	}

	float maxAnisotropy;

	size_t hash = 0;

	VkSampler sampler = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};

class ImageVulkan : public Image
{
public:

	ImageVulkan()
	{
	}

	virtual ~ImageVulkan()
	{
	}

public:

	VKImage* m_Image;
	VKImageSampler* m_ImageSampler;
};

class ImageManager : public NonCopyable
{
public:

	ImageManager(VkDevice vkDevice);
	~ImageManager();

	VKImage* GetImage(VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount, VkImageUsageFlags usage
		, VkImageViewType imageViewType, VkImageAspectFlags aspectMask);

	VKImageSampler* GetImageSampler(uint32_t mipLevels, float maxAnisotropy);

	void ReleaseImage(VKImage* image);

private:

	VKImage* CreateImage(VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount, VkImageUsageFlags usage
		, VkImageViewType imageViewType, VkImageAspectFlags aspectMask);

	VKImageSampler* CreateImageSampler(uint32_t mipLevels, float maxAnisotropy);

private:

	ResourcePoolOld<VKImage> m_ImagePool;

	std::unordered_map<size_t, VKImageSampler*> m_ImageSamplerPool;

	VkDevice m_Device = VK_NULL_HANDLE;
};