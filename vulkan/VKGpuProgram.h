#pragma once

#include "Env.h"
#include "GpuProgram.h"

class VKGpuProgram : public GpuProgram
{
public:

	VKGpuProgram(VkDevice vkDevice, GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode);
	~VKGpuProgram();

	VkShaderModule GetVertShaderModule();
	VkShaderModule GetFragShaderModule();

	VkDescriptorSetLayout GetDescriptorSetLayoutPerMaterial();
	VkDescriptorSetLayout GetDescriptorSetLayoutPerDraw();

private:

	VkShaderModule m_VertShaderModule = VK_NULL_HANDLE;
	VkShaderModule m_FragShaderModule = VK_NULL_HANDLE;

	VkDescriptorSetLayout m_DescriptorSetLayoutPerMaterial = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_DescriptorSetLayoutPerDraw = VK_NULL_HANDLE;

	VkDevice m_Device = VK_NULL_HANDLE;
};