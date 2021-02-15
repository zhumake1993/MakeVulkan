#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "GfxTypes.h"

class VKGpuProgram;

struct PipelineCI
{
	PipelineCI();
	~PipelineCI(); // �������麯����������memset��պ��麯����ָ��Ҳ�ᱻ���

	void Reset(VKGpuProgram* vkGpuProgram, RenderState& renderState, void* scdata, VkRenderPass renderPass);

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

	// ��ǰ��PipelineCI
	PipelineCI* m_PipelineCI = nullptr;

	GarbageCollector* m_GarbageCollector = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;

	// todo
	//VkPipelineCache pipelineCache = VK_NULL_HANDLE;
};