#pragma once

#include "Common.h"
#include "Tools.h"

class VulkanShaderModule;
class VulkanDevice;
class VulkanPipelineLayout;
class VulkanRenderPass;

enum VKShaderType
{
	kVKShaderVertex = 0,
	kVKShaderFragment = 1,
	kVKShaderCount
};

class PipelineCI
{

public:

	PipelineCI();

	void SetVertexInputState(const std::vector<VkFormat>& formats);
	void SetDynamicState(const std::vector<VkDynamicState>& states);

	// 只支持vs和ps
	VkPipelineShaderStageCreateInfo shaderStageCreateInfos[kVKShaderCount];

	std::vector<VkFormat> vertexFormats;
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

class VulkanPipeline
{

public:

	VulkanPipeline(VulkanDevice* vulkanDevice, PipelineCI& pipelineCI);
	~VulkanPipeline();

private:

	//

public:

	VkPipeline m_Pipeline = VK_NULL_HANDLE;
	VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};