#include "VKImage.h"

#include "DeviceProperties.h"
#include "Tools.h"
#include "VulkanDriver.h"

#include "VKDevice.h"

VKImage::VKImage(VKDevice* vkDevice, VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI):
	device(vkDevice->device),
	width(imageCI.extent.width),
	height(imageCI.extent.height)
{
	// Image

	VK_CHECK_RESULT(vkCreateImage(device, &imageCI, nullptr, &image));

	// Memory

	auto& driver = GetVulkanDriver();

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(device, image, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = driver.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory));

	// Bind

	VK_CHECK_RESULT(vkBindImageMemory(device, image, memory, 0));

	// View

	viewCI.image = image;
	viewCI.format = imageCI.format;
	VK_CHECK_RESULT(vkCreateImageView(device, &viewCI, nullptr, &view));
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

uint32_t VKImage::GetWidth()
{
	return width;
}

uint32_t VKImage::GetHeight()
{
	return height;
}
