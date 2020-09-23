#pragma once

#include "Common.h" 

class VulkanImage;
class VulkanBuffer;
class VulkanPipelineLayout;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanCommandBuffer;

class Imgui
{

	// test!!
	struct PushConstBlock {
		glm::vec2 scale;
		glm::vec2 translate;
	};

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

	VulkanImage* m_FontImage;

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