#include "VulkanImage.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanImage::VulkanImage(VulkanDevice * vulkanDevice):
	m_VulkanDevice(vulkanDevice)
{

}

VulkanImage::~VulkanImage()
{
}

void VulkanImage::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE) {
		if (m_Buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(m_VulkanDevice->m_LogicalDevice, m_Buffer, nullptr);
			m_Buffer = VK_NULL_HANDLE;
		}
		if (m_Memory != VK_NULL_HANDLE) {
			vkFreeMemory(m_VulkanDevice->m_LogicalDevice, m_Memory, nullptr);
			m_Memory = VK_NULL_HANDLE;
		}
	}
}

void VulkanImage::CreateImage()
{
}

void VulkanImage::AllocateMemory()
{
}
