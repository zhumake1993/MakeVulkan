#include "VKImage.h"

#include "DeviceProperties.h"
#include "Tools.h"
#include "VKDevice.h"

VKImage::VKImage(VKDevice* vkDevice)
	: device(vkDevice->device)
{
}

VKImage::~VKImage()
{
	if (device != VK_NULL_HANDLE) {
		if (image != VK_NULL_HANDLE) {
			vkDestroyImage(device, image, nullptr);
			image = VK_NULL_HANDLE;
		}
		if (memory != VK_NULL_HANDLE) {
			vkFreeMemory(device, memory, nullptr);
			memory = VK_NULL_HANDLE;
		}
		if (view != VK_NULL_HANDLE) {
			vkDestroyImageView(device, view, nullptr);
			view = VK_NULL_HANDLE;
		}
	}
}

void VKImage::CreateVkImage()
{
	VkImageCreateInfo ci = {};
	ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ci.pNext = nullptr;
	ci.flags = imageCreateFlags;
	ci.imageType = imageType;
	ci.format = format;
	ci.extent.width = width;
	ci.extent.height = height;
	ci.extent.depth = depth;
	ci.mipLevels = mipLevels;
	ci.arrayLayers = arrayLayers;
	ci.samples = samples;
	ci.tiling = tiling;
	ci.usage = usage;
	ci.sharingMode = sharingMode;
	ci.queueFamilyIndexCount = queueFamilyIndexCount;
	ci.pQueueFamilyIndices = pQueueFamilyIndices;
	ci.initialLayout = initialLayout;

	VK_CHECK_RESULT(vkCreateImage(device, &ci, nullptr, &image));

	auto& dp = GetDeviceProperties();

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(device, image, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = dp.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory));

	VK_CHECK_RESULT(vkBindImageMemory(device, image, memory, 0));
}

void VKImage::CreateVkImageView()
{
	VkImageViewCreateInfo ci = {};
	ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ci.pNext = nullptr;
	ci.flags = imageViewCreateFlags;
	ci.image = image;
	ci.viewType = viewType;
	ci.format = format;
	ci.components.r = rSwizzle;
	ci.components.g = gSwizzle;
	ci.components.b = bSwizzle;
	ci.components.a = aSwizzle;
	ci.subresourceRange.aspectMask = aspectMask;
	ci.subresourceRange.baseMipLevel = baseMipLevel;
	ci.subresourceRange.levelCount = levelCount;
	ci.subresourceRange.baseArrayLayer = baseArrayLayer;
	ci.subresourceRange.layerCount = layerCount;

	VK_CHECK_RESULT(vkCreateImageView(device, &ci, nullptr, &view));
}
