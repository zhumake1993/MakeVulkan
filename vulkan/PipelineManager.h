#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "GfxTypes.h"

struct PipelineCI
{
	PipelineCI();
	~PipelineCI(); // �������麯����������memset��պ��麯����ָ��Ҳ�ᱻ���

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

	// ��ǰ��PipelineCI
	PipelineCI* m_PipelineCI = nullptr;

	// ����Pipeline Layout Compatibility�����ԣ��ڴ���ʵ��ʹ�õ�PipelineLayout֮ǰ���Ϳ��԰�Global��PerView
	VkPipelineLayout m_DummyPipelineLayout = VK_NULL_HANDLE;

	GarbageCollector* m_GarbageCollector = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;

	// todo
	//VkPipelineCache pipelineCache = VK_NULL_HANDLE;
};