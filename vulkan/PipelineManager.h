#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "GfxTypes.h"

struct PipelineCI
{
	PipelineCI();
	~PipelineCI(); // 不能有虚函数，否则用memset清空后，虚函数表指针也会被清空

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

	VkPipelineLayout CreatePipelineLayout(std::vector<VkDescriptorSetLayout>& layouts);

	void SetDummyPipelineLayout(VkPipelineLayout layout);
	VkPipelineLayout GetDummyPipelineLayout();

	void SetPipelineCI(std::vector<VkDescriptorSetLayout>& layouts, VkRenderPass renderPass, RenderStatus& renderStatus, VkShaderModule vertexSM, VkShaderModule framentSM);

	VkPipelineLayout GetCurrPipelineLayout();

	VkPipeline CreatePipeline(VertexDescription& vertexDescription);

private:

	// 当前的PipelineCI
	PipelineCI* m_PipelineCI = nullptr;

	// 利用Pipeline Layout Compatibility的特性，在创建实际使用的PipelineLayout之前，就可以绑定Global和PerView
	VkPipelineLayout m_DummyPipelineLayout = VK_NULL_HANDLE;

	GarbageCollector* m_GarbageCollector = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;

	// todo
	//VkPipelineCache pipelineCache = VK_NULL_HANDLE;
};