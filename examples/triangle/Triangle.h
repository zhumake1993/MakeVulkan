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

private:

	// Mesh
	Mesh* m_Mesh;

	// Texture
	VulkanImage* m_Image;

	// RenderNode
	RenderNode* m_RenderNode;

	// DescriptorSet
	VulkanDescriptorPool* m_VulkanDescriptorPool;
	VulkanDescriptorSetLayout* m_VulkanDescriptorSetLayout;
	VulkanDescriptorSet* m_VulkanDescriptorSet;

	// Render Status
	VulkanPipeline* m_VulkanPipeline;
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