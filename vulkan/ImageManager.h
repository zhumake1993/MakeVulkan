#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "Image.h"
#include "VKResource.h"

struct VKImage : public VKResource
{
	VKImage(VkDevice vkDevice, VkImageType vkImageType, VkFormat vkFormat, uint32_t vkWidth, uint32_t vkHeight, uint32_t vkMipLevels, VkImageUsageFlags vkUsage) :
		device(vkDevice), imageType(vkImageType), format(vkFormat), width(vkWidth), height(vkHeight), mipLevels(vkMipLevels), usage(vkUsage) {}
	virtual ~VKImage()
	{
		vkDestroyImage(device, image, nullptr);
		vkFreeMemory(device, memory, nullptr);
	}

	VkImageType imageType;
	VkFormat format;
	uint32_t width;
	uint32_t height;
	uint32_t mipLevels;
	VkImageUsageFlags usage;

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;
};

struct VKImageView : public VKResource
{
	VKImageView(VkDevice vkDevice, VkImage vkImage, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask) :
		device(vkDevice), image(vkImage), imageViewType(vkImageViewType), format(vkFormat), aspectMask(vkAspectMask) {}
	virtual ~VKImageView()
	{
		vkDestroyImageView(device, view, nullptr);
	}

	VkImage image;
	VkImageViewType imageViewType;
	VkFormat format;
	VkImageAspectFlags aspectMask;

	VkImageView view = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;
};

struct VKImageSampler : public VKResource
{
	VKImageSampler(VkDevice vkDevice) :
		device(vkDevice)
	{
	}
	virtual ~VKImageSampler()
	{
		vkDestroySampler(device, sampler, nullptr);
	}

	VkSampler sampler = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;
};

class ImageImpl : public Image
{
public:

	ImageImpl(ImageType imageType, VkFormat format, uint32_t width, uint32_t height, VKImage* image, VKImageView* view, VKImageSampler* sampler);
	virtual ~ImageImpl();

	VKImage* GetImage() { return m_Image; }
	void SetImage(VKImage* image) { m_Image = image; }

	VKImageView* GetView() { return m_View; }
	void SetView(VKImageView* view) { m_View = view; }

	VKImageSampler* GetSampler() { return m_Sampler; }
	void SetSampler(VKImageSampler* sampler) { m_Sampler = sampler; }

private:

	VKImage* m_Image;
	VKImageView* m_View;
	VKImageSampler* m_Sampler;
};

class GarbageCollector;

class ImageManager : public NonCopyable
{
public:

	ImageManager(VkDevice vkDevice, GarbageCollector* gc);
	~ImageManager();

	VKImage* CreateImage(VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, VkImageUsageFlags usage);
	VKImageView* CreateView(VkImage image, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageAspectFlags vkAspectMask, uint32_t mipLevels);
	VKImageSampler* CreateSampler(uint32_t mipLevels, float maxAnisotropy);

private:

	GarbageCollector* m_GarbageCollector = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;
};