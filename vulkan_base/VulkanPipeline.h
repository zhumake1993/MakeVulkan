#pragma once

#include "VulkanCommon.h"
#include "Tools.h"

class VulkanShaderModule;

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

struct ShaderStage
{
	std::shared_ptr<VulkanShaderModule> vertShaderModule;
	std::string vertEntry = "main";
	std::shared_ptr<VulkanShaderModule> fragShaderModule;
	std::string fragEntry = "main";
};

enum VertexChannel {
	kVertexFormatFloat32x4,
	kVertexFormatFloat32x3,
	kVertexFormatFloat32x2,
};

struct VertexInputState
{
	std::vector<VertexChannel> vertexLayout;

	uint32_t GetStride() {
		uint32_t stride = 0;
		for (uint32_t i = 0; i < vertexLayout.size(); i++) {
			stride += GetSize(i);
		}
		return stride;
	}

	VkFormat GetVkFormat(uint32_t i) {
		assert(i < vertexLayout.size());
		switch (vertexLayout[i])
		{
		case kVertexFormatFloat32x4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case kVertexFormatFloat32x3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case kVertexFormatFloat32x2:
			return VK_FORMAT_R32G32_SFLOAT;
		default:
			LOG("invalid vertex channel!");
			assert(false);
			return VK_FORMAT_UNDEFINED;
		}
	}

	uint32_t GetSize(uint32_t i) {
		assert(i < vertexLayout.size());
		switch (vertexLayout[i])
		{
		case kVertexFormatFloat32x4:
			return 4 * sizeof(float);
		case kVertexFormatFloat32x3:
			return 3 * sizeof(float);
		case kVertexFormatFloat32x2:
			return 2 * sizeof(float);
		default:
			LOG("invalid vertex channel!");
			assert(false);
			return 0;
		}
	}
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

struct PipelineStatus
{
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
};

class VulkanDevice;
class VulkanPipelineLayout;
class VulkanRenderPass;

struct PipelineCreateInfo
{
	PipelineCreateInfo(PipelineStatus* pipelineStatus, std::shared_ptr<VulkanPipelineLayout> vulkanPipelineLayout, VulkanRenderPass* vulkanRenderPass);

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

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;

	PipelineStatus* m_PipelineStatus = nullptr;
};

class VulkanPipeline
{

public:

	VulkanPipeline(VulkanDevice* vulkanDevice, PipelineStatus* pipelineStatus, std::shared_ptr<VulkanPipelineLayout> vulkanPipelineLayout, VulkanRenderPass* vulkanRenderPass);
	~VulkanPipeline();

	void CleanUp();

private:

	//

public:

	VkPipeline m_Pipeline = VK_NULL_HANDLE;
	VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
};