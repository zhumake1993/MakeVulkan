#include "VKImage.h"
#include "Tools.h"
#include "VulkanDriver.h"

VKImage::VKImage(VkDevice device, VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI):
	m_Device(device),
	m_Width(imageCI.extent.width),
	m_Height(imageCI.extent.height)
{
	// Image

	VK_CHECK_RESULT(vkCreateImage(m_Device, &imageCI, nullptr, &m_Image));

	// Memory

	auto& driver = GetVulkanDriver();

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(m_Device, m_Image, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = driver.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &m_Memory));

	// Bind

	VK_CHECK_RESULT(vkBindImageMemory(m_Device, m_Image, m_Memory, 0));

	// View

	viewCI.image = m_Image;
	viewCI.format = imageCI.format;
	VK_CHECK_RESULT(vkCreateImageView(m_Device, &viewCI, nullptr, &m_View));
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

VkImage VKImage::GetImage()
{
	return m_Image;
}

VkDeviceMemory VKImage::GetMemory()
{
	return m_Memory;
}

VkImageView VKImage::GetView()
{
	return m_View;
}

uint32_t VKImage::GetWidth()
{
	return m_Width;
}

uint32_t VKImage::GetHeight()
{
	return m_Height;
}
