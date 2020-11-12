#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VKTypes.h"

struct VKDevice;
struct VKShaderModule;
struct VKKRenderPass;

struct PipelineCI
{

	PipelineCI();

	void SetVertexInputState(const VertexDescription& vertexDes);
	void SetDynamicState(const std::vector<VkDynamicState>& states);
	void SetSpecializationConstant(VKShaderType shaderType, VkSpecializationInfo& specializationInfo);

	// 只支持vs和ps
	VkPipelineShaderStageCreateInfo shaderStageCreateInfos[kVKShaderCount];

	// 只支持一个绑定点0
	VkVertexInputBindingDescription vertexInputBindings[1];
	// 只支持最多4个顶点属性
	VkVertexInputAttributeDescription vertexInputAttributs[4];
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo;
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;

private:

	void ConfigShaderStageCreateInfos();
	void ConfigVertexInputStateCreateInfo(const VertexDescription& vertexDes = {});
	void ConfigInputAssemblyStateCreateInfo();
	void ConfigTessellationStateCreateInfo();
	void ConfigViewportStateCreateInfo();
	void ConfigRasterizationStateCreateInfo();
	void ConfigMultisampleStateCreateInfo();
	void ConfigDepthStencilStateCreateInfo();
	void ConfigColorBlendStateCreateInfo();
	void ConfigDynamicStateCreateInfo();

	std::vector<VkDynamicState> dynamicStates;
};

struct VKPipeline : public NonCopyable
{
	VKPipeline(VKDevice* vkDevice, PipelineCI& pipelineCI);
	~VKPipeline();

	VkPipeline pipeline = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

private:

	VkPipelineCache pipelineCache = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
};