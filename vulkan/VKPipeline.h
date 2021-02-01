#pragma once

#include "Env.h"
#include "VKResource.h"
#include "GfxTypes.h"

struct PipelineCI
{
	PipelineCI();
	~PipelineCI();

	void Reset(VkPipelineLayout layout, VkRenderPass renderPass, RenderStatus& renderStatus, VkShaderModule vertexSM, VkShaderModule framentSM);

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;

	VkPipelineShaderStageCreateInfo shaderStageCreateInfos[kVKShaderTypeCount]; // 只支持vs和ps
	VkVertexInputBindingDescription vertexInputBindings[1]; // 只支持一个绑定点0
	VkVertexInputAttributeDescription vertexInputAttributs[4]; // 只支持最多4个顶点属性
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
};

struct VKPipeline : public VKResource
{
	VKPipeline(uint32_t currFrameIndex, VkDevice vkDevice, PipelineCI& pipelineCI, VertexDescription& vertexDescription);
	~VKPipeline();

	VkPipeline pipeline = VK_NULL_HANDLE;
	// todo
	//VkPipelineCache pipelineCache = VK_NULL_HANDLE;
};