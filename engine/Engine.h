#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "EngineFwd.h"
#include "DescriptorSetTypes.h"

class Imgui;
class GPUProfilerMgr;

struct Light
{
	alignas(16) glm::vec3 strength; // light color
	float falloffStart; // point/spot light only
	alignas(16) glm::vec3 direction;// directional/spot lightonly
	float falloffEnd; // point/spot light only
	alignas(16) glm::vec3 position; // point/spot light only
	float spotPower; // spot light only
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

	void UpdatePassUniformBuffer(void* data);
	void UpdateObjectUniformBuffer(RenderNode* node);
	void UpdateMaterialUniformBuffer(Material* material);

	Mesh* CreateMesh();
	Texture* CreateTexture();
	Shader* CreateShader();
	Material* CreateMaterial(std::string name);
	RenderNode* CreateRenderNode();

private:

	//

protected:

	// Uniform Buffer
	PassUniform m_PassUniform;

	Imgui* m_Imgui;

	uint32_t m_ObjectUBAlignment;

private:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;
};