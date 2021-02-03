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

	// 当前的PipelineCI
	PipelineCI* m_PipelineCI = nullptr;

	// 这一帧新加的DescriptorSet
	std::list<VKPipeline*> m_NewPipelines;

	// 可能还使用中的DescriptorSet
	std::list<VKPipeline*> m_PendingPipelines;

	// 利用Pipeline Layout Compatibility的特性，在创建实际使用的PipelineLayout之前，就可以绑定Global和PerView
	VkPipelineLayout m_DummyPipelineLayout = VK_NULL_HANDLE;

	uint32_t m_FrameIndex = 0;

	VkDevice m_Device = VK_NULL_HANDLE;
};