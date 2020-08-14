#pragma once

#include "VulkanCommon.h"

class VulkanDevice;

class VulkanFramebuffer
{

public:

	VulkanFramebuffer(VulkanDevice* vulkanDevice);
	~VulkanFramebuffer();

	void CleanUp();

private:

	//

public:

	VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};