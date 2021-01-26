#include "VKImage.h"
#include "DeviceProperties.h"
#include "VulkanTools.h"

VKImage::VKImage(ImageType imageType, VkDevice vkDevice, VkImageType vkImageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage,
	VkImageViewType vkImageViewType, VkImageAspectFlags aspectMask) :
	Image(imageType, format, width, height),
	m_Device(vkDevice)
{
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
	imageCI.mipLevels = 1;
	imageCI.arrayLayers = 1;
	imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCI.usage = usage;
	imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCI.queueFamilyIndexCount = 0;
	imageCI.pQueueFamilyIndices = nullptr;
	imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VK_CHECK_RESULT(vkCreateImage(m_Device, &imageCI, nullptr, &m_Image));

	// Memory

	auto& dp = GetDeviceProperties();

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(m_Device, m_Image, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = dp.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &m_Memory));

	// Bind

	VK_CHECK_RESULT(vkBindImageMemory(m_Device, m_Image, m_Memory, 0));

	// View

	VkImageViewCreateInfo imageViewCI = {};
	imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCI.pNext = nullptr;
	imageViewCI.flags = 0;
	imageViewCI.image = m_Image;
	imageViewCI.viewType = vkImageViewType;
	imageViewCI.format = format;
	imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.subresourceRange.aspectMask = aspectMask;
	imageViewCI.subresourceRange.baseMipLevel = 0;
	imageViewCI.subresourceRange.levelCount = 1;
	imageViewCI.subresourceRange.baseArrayLayer = 0;
	imageViewCI.subresourceRange.layerCount = 1;

	VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageViewCI, nullptr, &m_View));
}

VKImage::~VKImage()
{
	if (m_Device != VK_NULL_HANDLE) {
		if (m_Image != VK_NULL_HANDLE) {
			vkDestroyImage(m_Device, m_Image, nullptr);
			m_Image = VK_NULL_HANDLE;
		}
		if (m_Memory != VK_NULL_HANDLE) {
			vkFreeMemory(m_Device, m_Memory, nullptr);
			m_Memory = VK_NULL_HANDLE;
		}
		if (m_View != VK_NULL_HANDLE) {
			vkDestroyImageView(m_Device, m_View, nullptr);
			m_View = VK_NULL_HANDLE;
		}
	}
}