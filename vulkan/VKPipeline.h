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

	VkPipelineShaderStageCreateInfo shaderStageCreateInfos[kVKShaderTypeCount]; // ֻ֧��vs��ps
	VkVertexInputBindingDescription vertexInputBindings[1]; // ֻ֧��һ���󶨵�0
	VkVertexInputAttributeDescription vertexInputAttributs[4]; // ֻ֧�����4����������
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