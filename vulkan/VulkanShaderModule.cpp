#include "VulkanShaderModule.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanShaderModule::VulkanShaderModule(VulkanDevice * vulkanDevice, std::string const & filename):
	m_VulkanDevice(vulkanDevice)
{
	const std::vector<char> code = GetBinaryFileContents(filename);
	if (code.size() == 0) {
		assert(false);
	}

	VkShaderModuleCreateInfo moduleCreateInfo = {};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.flags = 0;
	moduleCreateInfo.codeSize = code.size();
	moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VK_CHECK_RESULT(vkCreateShaderModule(m_VulkanDevice->m_LogicalDevice, &moduleCreateInfo, nullptr, &m_ShaderModule));
}

VulkanShaderModule::~VulkanShaderModule()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_ShaderModule != VK_NULL_HANDLE) {
		vkDestroyShaderModule(m_VulkanDevice->m_LogicalDevice, m_ShaderModule, nullptr);
		m_ShaderModule = VK_NULL_HANDLE;
	}
}