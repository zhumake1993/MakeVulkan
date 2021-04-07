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

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};

struct ImageSamplerKey
{
	bool operator==(const ImageSamplerKey & other) const
	{
		return mipLevels == other.mipLevels
			&& maxAnisotropy == other.maxAnisotropy;
	}
	uint32_t mipLevels;
	float maxAnisotropy;
};

struct ImageSamplerKeyHash
{
	size_t operator()(const ImageSamplerKey & imageSamplerKey) const
	{
		return std::hash<uint32_t>()(imageSamplerKey.mipLevels)
			^ std::hash<float>()(imageSamplerKey.maxAnisotropy);
	}
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

	uint32_t m_MipLevels;
	float m_MaxAnisotropy;

	VkSampler sampler = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};

struct ImageKey
{
	bool operator==(const ImageKey & other) const
	{
		return imageType == other.imageType
			&& format == other.format
			&& width == other.width
			&& height == other.height
			&& mipLevels == other.mipLevels
			&& layerCount == other.layerCount
			&& faceCount == other.faceCount
			&& usage == other.usage
			&& imageViewType == other.imageViewType
			&& aspectMask == other.aspectMask;
	}
	VkImageType imageType;
	VkFormat format;
	uint32_t width;
	uint32_t height;
	uint32_t mipLevels;
	uint32_t layerCount;
	uint32_t faceCount;
	VkImageUsageFlags usage;
	VkImageViewType imageViewType;
	VkImageAspectFlags aspectMask;
};

struct ImageKeyHash
{
	size_t operator()(const ImageKey & imageKey) const
	{
		return std::hash<int>()(imageKey.imageType)
			^ std::hash<int>()(imageKey.format)
			^ std::hash<uint32_t>()(imageKey.width)
			^ std::hash<uint32_t>()(imageKey.height)
			^ std::hash<uint32_t>()(imageKey.mipLevels)
			^ std::hash<uint32_t>()(imageKey.layerCount)
			^ std::hash<uint32_t>()(imageKey.faceCount)
			^ std::hash<VkImageUsageFlags>()(imageKey.usage)
			^ std::hash<int>()(imageKey.imageViewType)
			^ std::hash<VkImageAspectFlags>()(imageKey.aspectMask);
	}
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

	ImageKey GetKey();

public:

	int m_ImageTypeMask = 0;

	// image
	VkImageType m_ImageType = VK_IMAGE_TYPE_2D;
	VkFormat m_Format = VK_FORMAT_UNDEFINED;
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	uint32_t m_MipLevels = 1;
	uint32_t m_LayerCount = 1;
	uint32_t m_FaceCount = 1;
	VkImageUsageFlags m_Usage = 0;

	// view
	// 简单起见，view的属性与image保持一致
	VkImageViewType m_ImageViewType = VK_IMAGE_VIEW_TYPE_2D;
	VkImageAspectFlags m_AspectMask = 0;

	VKImage* m_Image = nullptr;
	VKImageSampler* m_ImageSampler = nullptr;
};

class ImageManager : public NonCopyable
{
public:

	ImageManager(VkDevice vkDevice);
	~ImageManager();

	VKImage* GetImage(const ImageKey& key);

	VKImageSampler* GetImageSampler(const ImageSamplerKey& key);

	void ReleaseImage(const ImageKey& key, VKImage* image);

private:

	VKImage* CreateImage(const ImageKey& key);

	VKImageSampler* CreateImageSampler(const ImageSamplerKey& key);

private:

	ResourcePool<ImageKey, VKImage, ImageKeyHash> m_ImagePool;

	std::unordered_map<ImageSamplerKey, VKImageSampler*, ImageSamplerKeyHash> m_ImageSamplerPool;

	VkDevice m_Device = VK_NULL_HANDLE;
};