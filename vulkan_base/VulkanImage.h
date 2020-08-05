#pragma once

#include "VulkanCommon.h"

class VulkanDevice;

class VulkanImage
{

public:

	VulkanImage(VulkanDevice* vulkanDevice);
	~VulkanImage();

	void CleanUp();

private:

	void CreateImage();
	void AllocateMemory();

public:

	//

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};