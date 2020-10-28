#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "EngineFwd.h"
#include "DescriptorSetTypes.h"

class Imgui;
class GPUProfilerMgr;

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
	Material* CreateMaterial();
	RenderNode* CreateRenderNode();

private:

	//

protected:

	Imgui* m_Imgui;

private:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;
};