#pragma once

#include "Env.h"
#include "NonCopyable.h"

class Mesh;
class Texture;
class Shader;
class Material;
class RenderNode;
class Camera;
class Buffer;
class Imgui;
class TimeManager;
class RenderPass;

class Example : public NonCopyable
{
public:

	Example();
	virtual ~Example();

	virtual void ConfigDeviceProperties() = 0;
	virtual void Init();
	virtual void Release();
	virtual void Update();
	virtual void Draw() = 0;

protected:

	Mesh* CreateMesh(const std::string& name);
	Texture* CreateTexture(const std::string& name);
	Shader* CreateShader(const std::string& name);
	Material* CreateMaterial(const std::string& name);
	RenderNode* CreateRenderNode(const std::string& name);

	void BindGlobalUniformBuffer(void * data, uint64_t size);
	void BindPerViewUniformBuffer(void * data, uint64_t size);

	void SetShader(Shader* shader);

	void BindMaterial(Material* material);

	void DrawRenderNode(RenderNode* node);

	// 要求相同Material
	void DrawBatch(std::vector<RenderNode*> nodes);

	// Instancing
	void DrawInstanced(Mesh* mesh, Shader* shader, void* data, uint64_t size, uint32_t instanceCount);

	void UpdateImgui();
	void DrawImgui();

protected:

	TimeManager* m_TimeManager;

private:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;

	Shader* m_DummyShader;
	Imgui* m_Imgui = nullptr;
};