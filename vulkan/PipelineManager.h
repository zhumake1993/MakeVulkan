#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "GfxTypes.h"

class VKGpuProgram;

struct PipelineCI
{
	PipelineCI();
	~PipelineCI(); // 不能有虚函数，否则用memset清空后，虚函数表指针也会被清空

	void Reset(VKGpuProgram* vkGpuProgram, RenderState& renderState, void* scdata, VkRenderPass renderPass);

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
	VKPipeline(VkDevice vkDevice) :device(vkDevice) {}
	virtual ~VKPipeline()
	{
		vkDestroyPipeline(device, pipeline, nullptr);
	}

	VkPipeline pipeline = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;
};

class GarbageCollector;

class PipelineManager : public NonCopyable
{
public:

	PipelineManager(VkDevice vkDevice, GarbageCollector* gc);
	virtual ~PipelineManager();

	void SetPipelineCI(VKGpuProgram* vkGpuProgram, RenderState& renderState, void* scdata, VkRenderPass renderPass);

	VkPipeline CreatePipeline(VertexDescription& vertexDescription);

private:

	// 当前的PipelineCI
	PipelineCI* m_PipelineCI = nullptr;

	GarbageCollector* m_GarbageCollector = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;

	// todo
	//VkPipelineCache pipelineCache = VK_NULL_HANDLE;
};