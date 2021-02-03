#pragma once

#include "Env.h"
#include "NonCopyable.h"
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
	VKPipeline(uint32_t currFrameIndex) :VKResource(currFrameIndex) {}
	~VKPipeline() {}

	VkPipeline pipeline = VK_NULL_HANDLE;

	// todo
	//VkPipelineCache pipelineCache = VK_NULL_HANDLE;
};

class PipelineManager : public NonCopyable
{
public:

	PipelineManager(VkDevice vkDevice);
	~PipelineManager();

	void Update();

	VkPipelineLayout CreatePipelineLayout(std::vector<VkDescriptorSetLayout>& layouts);

	void SetDummyPipelineLayout(VkPipelineLayout layout);
	VkPipelineLayout GetDummyPipelineLayout();

	void SetPipelineCI(std::vector<VkDescriptorSetLayout>& layouts, VkRenderPass renderPass, RenderStatus& renderStatus, VkShaderModule vertexSM, VkShaderModule framentSM);

	VkPipelineLayout GetCurrPipelineLayout();

	VkPipeline CreatePipeline(VertexDescription& vertexDescription);

private:

	// ��ǰ��PipelineCI
	PipelineCI* m_PipelineCI = nullptr;

	// ��һ֡�¼ӵ�DescriptorSet
	std::list<VKPipeline*> m_NewPipelines;

	// ���ܻ�ʹ���е�DescriptorSet
	std::list<VKPipeline*> m_PendingPipelines;

	// ����Pipeline Layout Compatibility�����ԣ��ڴ���ʵ��ʹ�õ�PipelineLayout֮ǰ���Ϳ��԰�Global��PerView
	VkPipelineLayout m_DummyPipelineLayout = VK_NULL_HANDLE;

	uint32_t m_FrameIndex = 0;

	VkDevice m_Device = VK_NULL_HANDLE;
};