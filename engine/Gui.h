#pragma once

#include "Common.h" 

class VulkanBuffer;
class VKImage;
class VKSampler;
class VulkanPipelineLayout;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanCommandBuffer;

class Imgui
{

public:

	Imgui(VulkanRenderPass* renderpass);
	~Imgui();

	void Tick();
	void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer);

private:

	//

public:

	//

private:

	VKImage* m_FontImage;
	VKSampler* m_Sampler;

	uint32_t m_MaxVertexCount = 65536;
	uint32_t m_MaxIndexCount = 65536;
	VulkanBuffer* m_VertexBuffer;
	VulkanBuffer* m_IndexBuffer;

	VkDescriptorSet m_DescriptorSet;
	VulkanPipelineLayout* m_VulkanPipelineLayout;
	VulkanPipeline* m_VulkanPipeline;

	// test
	VkPipelineLayout m_PipelineLayout;
};