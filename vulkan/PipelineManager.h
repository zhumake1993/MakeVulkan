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

	void Reset(VKGpuProgram* vkGpuProgram, RenderState* renderState, void* scdata, VkRenderPass renderPass, uint32_t subPassIndex);

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

	// Ϊ�˷��㣬ֱ��ʹ��ָ����Ϊkey
	// ��ֻ��һ�ֲִڵ�cache��ʽ�����ܶ�̬�޸ģ�
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

	// ��ǰ��PipelineKey
	PipelineKey m_PipelineKey;

	std::unordered_map<PipelineKey, Pipeline*, PipelineHash> m_PSOCache;

	uint32_t m_FrameIndex = 0;

	VkDevice m_Device = VK_NULL_HANDLE;

	VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
};