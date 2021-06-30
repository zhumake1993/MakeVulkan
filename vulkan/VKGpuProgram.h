#pragma once

#include "VKIncludes.h"
#include "GpuProgram.h"

class VKGpuProgram : public GpuProgram
{
public:

	VKGpuProgram(VkDevice vkDevice, GpuParameters& parameters, const mkVector<char>& vertCode, const mkVector<char>& fragCode);
	virtual ~VKGpuProgram();

	VkShaderModule GetVertShaderModule();
	VkShaderModule GetFragShaderModule();

	static VkDescriptorSetLayout GetDSLGlobal();
	static VkDescriptorSetLayout GetDSLPerView();

	VkDescriptorSetLayout GetDSLPerMaterial();
	VkDescriptorSetLayout GetDSLPerDraw();

	VkPipelineLayout GetPipelineLayout();

	VkSpecializationInfo& GetSpecializationInfo();

private:

	VkShaderModule m_VertShaderModule = VK_NULL_HANDLE;
	VkShaderModule m_FragShaderModule = VK_NULL_HANDLE;

	static VkDescriptorSetLayout m_DSLGlobal;
	static VkDescriptorSetLayout m_DSLPerView;

	VkDescriptorSetLayout m_DSLPerMaterial = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_DSLPerDraw = VK_NULL_HANDLE;

	// 利用Pipeline Layout Compatibility的特性，在创建实际使用的PipelineLayout之前，就可以绑定Global和PerView
	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

	// SpecializationConstant
	mkVector<VkSpecializationMapEntry> m_SpecializationMapEntries;
	VkSpecializationInfo m_SpecializationInfo;

	VkDevice m_Device = VK_NULL_HANDLE;
};