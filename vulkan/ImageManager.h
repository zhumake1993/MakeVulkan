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
			^ std::hash<VkImageUsageFlags>()(usage);

		return hash;
	}

	VkImageType imageType;
	VkFormat format;
	uint32_t width;
	uint32_t height;
	uint32_t mipLevels;
	uint32_t layerCount;
	uint32_t faceCount;
	VkImageUsageFlags usage;

	size_t hash = 0;

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};

class VKImageView : public VKResource
{
public:

	VKImageView(VkDevice vkDevice)
		: device(vkDevice)
	{
	}

	virtual ~VKImageView()
	{
		vkDestroyImageView(device, view, nullptr);
	}

	VkImageViewType imageViewType;
	VkImageAspectFlags vkAspectMask;

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

	float maxAnisotropy;

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
	VKImageView* m_ImageView;
	VKImageSampler* m_ImageSampler;
};

class ImageManager : public NonCopyable
{
public:

	ImageManager(VkDevice vkDevice);
	~ImageManager();

	VKImage* GetImage(VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount, VkImageUsageFlags usage);
	VKImageView* GetImageView(VkImage image, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount);
	VKImageSampler* GetImageSampler(uint32_t mipLevels, float maxAnisotropy);

	void ReleaseImage(VKImage* image);
	//VKImageView* ReleaseImageView(VkImage image, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount);

private:

	VKImage* CreateImage(VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount, VkImageUsageFlags usage);
	VKImageView* CreateImageView(VkImage image, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount);
	VKImageSampler* CreateImageSampler(uint32_t mipLevels, float maxAnisotropy);

private:

	ResourcePool<VKImage> m_ImagePool;


	VkDevice m_Device = VK_NULL_HANDLE;
};