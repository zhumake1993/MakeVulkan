#pragma once

#include "Env.h"
#include "NonCopyable.h"

class Mesh;
class Texture;

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

	Mesh* CreateMesh();
	Texture* CreateTexture();
	//Shader* CreateShader();
	//Material* CreateMaterial(std::string name);
	//RenderNode* CreateRenderNode();

protected:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	//std::vector<Shader*> m_ShaderContainer;
	//std::vector<Material*> m_MaterialContainer;
	//std::vector<RenderNode*> m_RenderNodeContainer;
};