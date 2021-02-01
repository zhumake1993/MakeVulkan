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

class Example : public NonCopyable
{
public:

	Example();
	virtual ~Example();

	virtual void ConfigDeviceProperties() = 0;
	virtual void Init() = 0;
	virtual void Release() = 0;
	virtual void Update() = 0;

protected:

	Mesh* CreateMesh(std::string name);
	Texture* CreateTexture(std::string name);
	Shader* CreateShader(std::string name);
	Material* CreateMaterial(std::string name);
	RenderNode* CreateRenderNode(std::string name);

protected:

	UniformDataGlobal m_UniformDataGlobal;
	UniformDataPerView m_UniformDataPerView;

private:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;

	Shader* m_DummyShader;
};