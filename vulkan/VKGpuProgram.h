#pragma once

#include "Env.h"
#include "GpuProgram.h"

class VKGpuProgram : public GpuProgram
{
public:

	VKGpuProgram(VkDevice vkDevice, GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode);
	virtual ~VKGpuProgram();

	VkShaderModule GetVertShaderModule();
	VkShaderModule GetFragShaderModule();

	VkDescriptorSetLayout& GetDSLPerMaterial();
	VkDescriptorSetLayout& GetDSLPerDraw();

	

private:

	VkShaderModule m_VertShaderModule = VK_NULL_HANDLE;
	VkShaderModule m_FragShaderModule = VK_NULL_HANDLE;

	VkDescriptorSetLayout m_DSLPerMaterial = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_DSLPerDraw = VK_NULL_HANDLE;

	VkDevice m_Device = VK_NULL_HANDLE;
};