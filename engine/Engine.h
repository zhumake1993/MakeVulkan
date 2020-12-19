#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "EngineFwd.h"
#include "DescriptorSetTypes.h"
#include "VKTypes.h"

class Imgui;

struct Light
{
	alignas(16) glm::vec3 strength; // light color
	float falloffStart; // point/spot light only
	alignas(16) glm::vec3 direction;// directional/spot lightonly
	float falloffEnd; // point/spot light only
	alignas(16) glm::vec3 position; // point/spot light only
	float spotPower; // spot light only
};

struct SpecializationData
{
	uint32_t numDirLights = 0;
	uint32_t numPointLights = 0;
	uint32_t numSpotLights = 0;
};

struct PassUniform {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;

	alignas(16) glm::vec4 ambientLight;
	alignas(16) Light lights[16];
};

struct ObjectUniform {
	alignas(16) glm::mat4 world;
};

class Engine : public NonCopyable
{

public:

	Engine();
	virtual ~Engine();

	void CleanUpEngine();
	void InitEngine();
	void TickEngine();
	virtual void RecordCommandBuffer(VKCommandBuffer* vkCommandBuffer) = 0;

protected:

	virtual void CleanUp() = 0;
	virtual void Init() = 0;
	virtual void Tick() = 0;
	virtual void TickUI() = 0;

	Mesh* CreateMesh();
	Texture* CreateTexture();
	Shader* CreateShader();
	Material* CreateMaterial(std::string name);
	RenderNode* CreateRenderNode();

	void CreateVKPipeline(Material* mat, const VertexDescription& vertexDescription);

	void DrawRenderNode(VKCommandBuffer * cb, RenderNode* renderNode);

private:

	void UpdateUniformBuffer();

protected:

	// Uniform Buffer
	PassUniform m_PassUniform;

	SpecializationData m_SpecializationData;

	Imgui* m_Imgui = nullptr;

	uint32_t m_ObjectUBAlignment;

	// DescriptorSet
	VkDescriptorSetLayout m_DSLPassUniform;
	VkDescriptorSetLayout m_DSLObjectDUB;

	VKRenderPass* m_VKRenderPass = nullptr;

	VKSampler* m_Sampler;

private:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;

	std::vector<VKPipeline*> m_VKPipelineContainer;

	Material* m_CurrMaterial = nullptr;

	VkPipelineLayout m_BasePipelineLayout = VK_NULL_HANDLE;

	VkDescriptorSet m_CurrDescriptorSetObjectDUB = VK_NULL_HANDLE;
};