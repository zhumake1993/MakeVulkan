#pragma once

#include <utility>
#include "VulkanCommon.h"

class VulkanDevice;

class VulkanShaderModule
{

public:

	VulkanShaderModule(VulkanDevice* vulkanDevice);
	~VulkanShaderModule();

	VulkanShaderModule(VulkanShaderModule&& other) {
		m_ShaderModule = other.m_ShaderModule;
		m_VulkanDevice = other.m_VulkanDevice;
		other.m_ShaderModule = VK_NULL_HANDLE;
		other.m_VulkanDevice = nullptr;
	}

	VulkanShaderModule& operator=(VulkanShaderModule other) {
		std::swap(m_ShaderModule, other.m_ShaderModule);
		std::swap(m_VulkanDevice, other.m_VulkanDevice);
		return *this;
	}

private:

	//

public:

	VkShaderModule m_ShaderModule = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};

class VulkanPipelineLayout
{

public:

	VulkanPipelineLayout();
	VulkanPipelineLayout(VulkanDevice* vulkanDevice);
	~VulkanPipelineLayout();

	//

private:

	//

public:

	VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};