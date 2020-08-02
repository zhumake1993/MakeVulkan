#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanShaderModule.h"

VulkanShaderModule::VulkanShaderModule(VulkanDevice* vulkanDevice) :m_VulkanDevice(vulkanDevice)
{
}

VulkanShaderModule::VulkanShaderModule(VulkanDevice * vulkanDevice, std::string const & filename)
{
}

VulkanShaderModule::~VulkanShaderModule()
{
	if (m_VulkanDevice && **m_VulkanDevice != VK_NULL_HANDLE && m_ShaderModule != VK_NULL_HANDLE) {
		vkDestroyShaderModule(**m_VulkanDevice, m_ShaderModule, nullptr);
	}
}

VulkanPipelineLayout::VulkanPipelineLayout()
{
}

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* vulkanDevice) :m_VulkanDevice(vulkanDevice)
{
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	if (m_VulkanDevice && **m_VulkanDevice != VK_NULL_HANDLE && m_VkPipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(**m_VulkanDevice, m_VkPipelineLayout, nullptr);
	}
}
