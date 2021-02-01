#pragma once

#include "Env.h"
#include "GpuProgram.h"

class VKGpuProgram : public GpuProgram
{
public:

	VKGpuProgram(VkDevice vkDevice, GpuParameters& parameters);
	~VKGpuProgram();

private:

	//VkDescriptorSetLayout m_DescriptorSetLayout0 = VK_NULL_HANDLE;

	// Uniform buffer
	VkDescriptorSetLayout m_DescriptorSetLayout1 = VK_NULL_HANDLE;

	VkDevice m_Device = VK_NULL_HANDLE;
};