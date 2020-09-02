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
	kVKShaderTessControl = 2,
	kVKShaderTessEval = 3,
	kVKShaderGeometry = 4,
	kVKShaderRayTracing = 5,
	kVKShaderCount
};

struct PipelineCI
{

	struct ShaderStage
	{
		std::shared_ptr<VulkanShaderModule> vertShaderModule;
		std::string vertEntry = "main";
		std::shared_ptr<VulkanShaderModule> fragShaderModule;
		std::string fragEntry = "main";
	};

	struct VertexInputState
	{
		std::vector<VkFormat> formats;
	};

	struct InputAssemblyState
	{
		VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	};

	struct TessellationState
	{
		//
	};

	struct ViewportState
	{
		//
	};

	struct RasterizationState
	{
		VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
		VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
	};

	struct MultisampleState
	{
		//
	};

	struct DepthStencilState
	{
		//
	};

	struct ColorBlendState
	{
		//
	};

	struct DynamicState
	{
		std::vector<VkDynamicState> dynamicStates;
	};

	ShaderStage shaderStage;
	VertexInputState vertexInputState;
	InputAssemblyState inputAssemblyState;
	TessellationState tessellationState;
	ViewportState viewportState;
	RasterizationState rasterizationState;
	MultisampleState multisampleState;
	DepthStencilState depthStencilState;
	ColorBlendState colorBlendState;
	DynamicState dynamicState;

	void Configure(VulkanPipelineLayout* vulkanPipelineLayout, VulkanRenderPass* vulkanRenderPass);

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

	VkPipelineShaderStageCreateInfo shaderStageCreateInfos[kVKShaderCount];
	VkVertexInputBindingDescription vertexInputBindings[1];
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