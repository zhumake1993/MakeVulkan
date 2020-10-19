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
	void TickUI() override;
	void RecordCommandBuffer(VKCommandBuffer* vkCommandBuffer) override;

private:

	void PrepareResources();
	void PrepareDescriptorSet();
	void CreatePipeline();

private:

	// Mesh
	Mesh* m_Home;
	Mesh* m_Cube;

	// Texture
	VKImage* m_HomeImage;
	VKImage* m_BrickImage;
	VKSampler* m_Sampler;

	// RenderNode
	RenderNode* m_HomeNode;
	RenderNode* m_CubeNode1;
	RenderNode* m_CubeNode2;

	// DescriptorSet
	VkDescriptorSetLayout m_DescriptorSetLayout;

	// Render Status
	VKPipeline* m_TexPipeline;
	VKPipeline* m_ColorPipeline;
	VKPipelineLayout* m_PipelineLayout;
	VKRenderPass* m_VKRenderPass;

	// Uniform Buffer
	PassUniform m_PassUniform;

	// camera
	Camera* m_Camera;

	// depth
	VkFormat m_DepthFormat;
	VKImage* m_DepthImage;
};