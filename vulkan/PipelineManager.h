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

	void Reset(VKGpuProgram* vkGpuProgram, RenderState* renderState, void* scdata, VkRenderPass renderPass, uint32_t subPassIndex);

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
	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
};

struct PipelineKey
{
	PipelineKey()
	{
		memset(this, 0, sizeof(*this));
	}

	bool operator==(const PipelineKey & p) const
	{
		return vkGpuProgram == p.vkGpuProgram
			&& renderState == p.renderState
			&& scdata == p.scdata
			&& renderPass == p.renderPass
			&& subPassIndex == p.subPassIndex
			&& vertexDescription == p.vertexDescription;
	}

	// 为了方便，直接使用指针作为key
	// 这只是一种粗糙的cache方式（不能动态修改）
	VKGpuProgram* vkGpuProgram;
	RenderState* renderState;
	void* scdata;
	VkRenderPass renderPass;
	uint32_t subPassIndex;
	VertexDescription* vertexDescription;
};

struct PipelineHash
{
	size_t operator()(const PipelineKey & p) const
	{
		return std::hash<void*>()(p.vkGpuProgram)
			^ std::hash<void*>()(p.renderState)
			^ std::hash<void*>()(p.scdata)
			^ std::hash<VkRenderPass>()(p.renderPass)
			^ std::hash<uint32_t>()(p.subPassIndex)
			^ std::hash<void*>()(p.vertexDescription);
	}
};

class PipelineManager : public NonCopyable
{

	struct Pipeline : public VKResource
	{
		Pipeline() {}
		~Pipeline() {}

		VkPipeline pipeline = VK_NULL_HANDLE;
	};

public:

	PipelineManager(VkDevice vkDevice);
	virtual ~PipelineManager();

	void Update();

	void SetPipelineKey(VKGpuProgram* vkGpuProgram, RenderState* renderState, void* scdata, VkRenderPass renderPass, uint32_t subPassIndex);

	VkPipeline CreatePipeline(VertexDescription* vertexDescription);

private:

	Pipeline* CreatePipelineInternal(PipelineKey& pipelineKey);

private:

	// 当前的PipelineKey
	PipelineKey m_PipelineKey;

	std::unordered_map<PipelineKey, Pipeline*, PipelineHash> m_PSOCache;

	uint32_t m_FrameIndex = 0;

	VkDevice m_Device = VK_NULL_HANDLE;

	VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
};