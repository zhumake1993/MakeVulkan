#pragma once

#include "Common.h"

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

	VkInstance m_Instance = VK_NULL_HANDLE;
};