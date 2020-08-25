#pragma once

#include "Common.h"

class VulkanDevice;

class VulkanShaderModule
{

public:

	VulkanShaderModule(VulkanDevice* vulkanDevice, std::string const &filename);
	~VulkanShaderModule();

private:

	//

public:

	VkShaderModule m_ShaderModule = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};