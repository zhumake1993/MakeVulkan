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
	Texture* m_HomeTex;
	Texture* m_Crate01Tex;
	Texture* m_Crate02Tex;
	VKSampler* m_Sampler;

	// Shader
	Shader* m_Shader;

	// Material
	Material* m_HomeMat;
	Material* m_Crate01Mat;
	Material* m_Crate02Mat;

	// RenderNode
	RenderNode* m_HomeNode;
	RenderNode* m_CubeNode1;
	RenderNode* m_CubeNode2;

	// DescriptorSet
	VkDescriptorSetLayout m_DSLPerDrawcall;
	VkDescriptorSetLayout m_DSLDynamicUBO;
	VkDescriptorSetLayout m_DSLHome;
	VkDescriptorSetLayout m_DSLCube;

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