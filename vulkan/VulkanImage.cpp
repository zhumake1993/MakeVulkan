#include "VulkanImage.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanImage::VulkanImage(VulkanDevice* vulkanDevice, VkImageType imageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage, VkImageAspectFlags aspect):
	m_VulkanDevice(vulkanDevice),
	m_ImageType(imageType),
	m_Format(format),
	m_Width(width),
	m_Height(height),
	m_Usage(usage),
	m_Aspect(aspect)
{
	CreateImage();
	AllocateMemory();
	VK_CHECK_RESULT(vkBindImageMemory(m_VulkanDevice->m_LogicalDevice, m_Image, m_Memory, 0));
	CreateImageView();
	CreateSampler();
}

VulkanImage::~VulkanImage()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE) {
		if (m_Sampler != VK_NULL_HANDLE) {
			vkDestroySampler(m_VulkanDevice->m_LogicalDevice, m_Sampler, nullptr);
			m_Sampler = VK_NULL_HANDLE;
		}
		if (m_ImageView != VK_NULL_HANDLE) {
			vkDestroyImageView(m_VulkanDevice->m_LogicalDevice, m_ImageView, nullptr);
			m_ImageView = VK_NULL_HANDLE;
		}
		if (m_Image != VK_NULL_HANDLE) {
			vkDestroyImage(m_VulkanDevice->m_LogicalDevice, m_Image, nullptr);
			m_Image = VK_NULL_HANDLE;
		}
		if (m_Memory != VK_NULL_HANDLE) {
			vkFreeMemory(m_VulkanDevice->m_LogicalDevice, m_Memory, nullptr);
			m_Memory = VK_NULL_HANDLE;
		}
	}
}

VkDescriptorImageInfo VulkanImage::GetVkDescriptorImageInfo()
{
	VkDescriptorImageInfo descriptorImageInfo = {};
	descriptorImageInfo.sampler = m_Sampler;
	descriptorImageInfo.imageView = m_ImageView;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	return descriptorImageInfo;
}

void VulkanImage::CreateImage()
{
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = m_ImageType;
	imageCreateInfo.format = m_Format;
	imageCreateInfo.extent = { m_Width,m_Height,1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = m_Usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VK_CHECK_RESULT(vkCreateImage(m_VulkanDevice->m_LogicalDevice, &imageCreateInfo, nullptr, &m_Image));
}

void VulkanImage::AllocateMemory()
{
	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(m_VulkanDevice->m_LogicalDevice, m_Image, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	memoryAllocateInfo.memoryTypeIndex = m_VulkanDevice->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_VulkanDevice->m_LogicalDevice, &memoryAllocateInfo, nullptr, &m_Memory));
}

void VulkanImage::CreateImageView()
{
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = m_Image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;// ÏÈÄ¬ÈÏ°É
	imageViewCreateInfo.format = m_Format;
	imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY };
	imageViewCreateInfo.subresourceRange = { m_Aspect,0,1,0,1 };

	VK_CHECK_RESULT(vkCreateImageView(m_VulkanDevice->m_LogicalDevice, &imageViewCreateInfo, nullptr, &m_ImageView));
}

void VulkanImage::CreateSampler()
{
	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = nullptr;
	samplerCreateInfo.flags = 0;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VK_CHECK_RESULT(vkCreateSampler(m_VulkanDevice->m_LogicalDevice, &samplerCreateInfo, nullptr, &m_Sampler));
}
