#pragma once

#include "VulkanCommon.h"

class VulkanInstance
{
	
public:

	VulkanInstance();
	~VulkanInstance();

	void CleanUp();

private:

	void ConfigInstanceLayers();
	void ConfigInstanceExtensions();

public:

	std::vector<VkLayerProperties> m_AvailableInstanceLayers;
	std::vector<VkExtensionProperties> m_AvailableInstanceExtensions;

	VkInstance m_Instance = VK_NULL_HANDLE;
};