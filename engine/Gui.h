#pragma once

#include "Common.h" 
#include "VulkanFwd.h"

class Imgui
{

public:

	Imgui();
	~Imgui();

	void Prepare(float deltaTime);
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
	VKPipelineLayout* m_PipelineLayout;
	VulkanPipeline* m_VulkanPipeline;

	VulkanRenderPass* m_RenderPass;
};