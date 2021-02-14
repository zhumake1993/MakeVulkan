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

struct UniformDataGlobal
{
	alignas(16) glm::vec4 time;
};

struct UniformDataPerView
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;
};

class TimeManager;

class Example : public NonCopyable
{
public:

	Example();
	virtual ~Example();

	virtual void ConfigDeviceProperties() = 0;
	virtual void Init();
	virtual void Release();
	virtual void Update();

protected:

	Mesh* CreateMesh(const std::string& name);
	Texture* CreateTexture(const std::string& name);
	Shader* CreateShader(const std::string& name);
	Material* CreateMaterial(const std::string& name);
	RenderNode* CreateRenderNode(const std::string& name);

	void SetShader(Shader* shader);
	void BindMaterial(Material* material);
	void DrawRenderNode(RenderNode* node);

protected:

	UniformDataGlobal m_UniformDataGlobal;
	UniformDataPerView m_UniformDataPerView;
	Shader* m_DummyShader;
	TimeManager* m_TimeManager;

private:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;

	
};