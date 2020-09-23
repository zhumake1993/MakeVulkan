#pragma once

#include "Engine.h"
#include "VulkanFwd.h"
#include "EngineFwd.h"

class Triangle : public Engine
{

public:

	Triangle();
	virtual ~Triangle();

protected:

	void CleanUp() override;
	void Init() override;
	void Tick() override;
	void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer) override;

private:

	void PrepareResources();
	void PrepareDescriptorSet();
	void CreatePipeline();
	void UpdateUI(float deltaTime);

private:

	// Mesh
	Mesh* m_Home;
	Mesh* m_Cube;

	// Texture
	VulkanImage* m_Image;

	// RenderNode
	RenderNode* m_HomeNode;
	RenderNode* m_CubeNode1;
	RenderNode* m_CubeNode2;

	// DescriptorSet
	VkDescriptorSetLayout m_DescriptorSetLayout;

	// Render Status
	VulkanPipeline* m_TexPipeline;
	VulkanPipeline* m_ColorPipeline;
	VulkanPipelineLayout* m_VulkanPipelineLayout;
	VulkanRenderPass* m_VulkanRenderPass;

	// Uniform Buffer
	PassUniform m_PassUniform;

	// camera
	Camera* m_Camera;

	// stats
	uint32_t m_FrameIndex = 0;
	uint32_t m_AccumulateCounter = 0;
	float m_AccumulateTime = 0;
	float m_FPS = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp = std::chrono::high_resolution_clock::now();

	// depth
	VkFormat m_DepthFormat;
	VulkanImage* m_DepthImage;
};