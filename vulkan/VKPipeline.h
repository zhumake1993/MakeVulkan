#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;
struct VKShaderModule;
struct VKPipelineLayout;
struct VKKRenderPass;

enum VKShaderType
{
	kVKShaderVertex = 0,
	kVKShaderFragment = 1,
	kVKShaderCount
};

struct PipelineCI
{

	PipelineCI();

	void SetVertexInputState(const std::vector<VkFormat>& formats);
	void SetDynamicState(const std::vector<VkDynamicState>& states);

	// ֻ֧��vs��ps
	VkPipelineShaderStageCreateInfo shaderStageCreateInfos[kVKShaderCount];

	std::vector<VkFormat> vertexFormats;
	// ֻ֧��һ���󶨵�0
	VkVertexInputBindingDescription vertexInputBindings[1];
	// ֻ֧�����4����������
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

	std::vector<VkDynamicState> dynamicStates;
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;

private:

	void ConfigShaderStageCreateInfos();
	void ConfigVertexInputStateCreateInfo();
	void ConfigInputAssemblyStateCreateInfo();
	void ConfigTessellationStateCreateInfo();
	void ConfigViewportStateCreateInfo();
	void ConfigRasterizationStateCreateInfo();
	void ConfigMultisampleStateCreateInfo();
	void ConfigDepthStencilStateCreateInfo();
	void ConfigColorBlendStateCreateInfo();
	void ConfigDynamicStateCreateInfo();
};

struct VKPipeline : public NonCopyable
{
	VKPipeline(VKDevice* vkDevice, PipelineCI& pipelineCI);
	~VKPipeline();

	VkPipeline pipeline = VK_NULL_HANDLE;

private:

	VkPipelineCache pipelineCache = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
};