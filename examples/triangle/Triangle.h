#pragma once

#include "Engine.h"
#include "VulkanFwd.h"

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

	// Test
	float m_Temp=0;

	// Mesh
	Mesh* m_Home;
	Mesh* m_Cube;
	Mesh* m_SimpleCube;
	Mesh* m_Sphere;

	// Texture
	Texture* m_HomeTex;
	Texture* m_Crate01Tex;
	Texture* m_Crate02Tex;
	VKSampler* m_Sampler;

	// Shader
	Shader* m_Shader;
	Shader* m_SimpleShader;

	// Material
	Material* m_HomeMat;
	Material* m_Crate01Mat;
	Material* m_Crate02Mat;
	Material* m_SimpleColorMat;

	// RenderNode
	RenderNode* m_HomeNode;
	RenderNode* m_CubeNode1;
	RenderNode* m_SphereNode;
	RenderNode* m_ColorCubeNode;

	// DescriptorSet
	VkDescriptorSetLayout m_DSLPassUniform;
	VkDescriptorSetLayout m_DSLObjectDUB;
	VkDescriptorSetLayout m_DSLMaterialDUB;
	VkDescriptorSetLayout m_DSLOneTex;

	// Render Status
	VKPipeline* m_TexPipeline;
	VKPipeline* m_ColorPipeline;
	VKPipelineLayout* m_PipelineLayout;
	VKRenderPass* m_VKRenderPass;

	// Uniform Buffer
	PassUniform m_PassUniform;

	// camera
	Camera* m_Camera;
};