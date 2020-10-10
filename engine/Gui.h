#pragma once

#include "Common.h" 
#include "VulkanFwd.h"

class Imgui
{

public:

	Imgui();
	~Imgui();

	void Prepare();
	void Tick();
	void RecordCommandBuffer(VKCommandBuffer* vkCommandBuffer);

private:

	//

public:

	//

private:

	VKImage* m_FontImage;
	VKSampler* m_Sampler;

	uint32_t m_MaxVertexCount = 65536;
	uint32_t m_MaxIndexCount = 65536;
	VKBuffer* m_VertexBuffer;
	VKBuffer* m_IndexBuffer;

	VkDescriptorSet m_DescriptorSet;
	VKPipelineLayout* m_PipelineLayout;
	VKPipeline* m_VulkanPipeline;

	VKRenderPass* m_RenderPass;
};