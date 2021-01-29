#include "Example.h"
#include "Tools.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "RenderNode.h"

Example::Example()
{
}

Example::~Example()
{
	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	for (auto p : m_ShaderContainer) { RELEASE(p); }
	for (auto p : m_MaterialContainer) { RELEASE(p); }
	for (auto p : m_RenderNodeContainer) { RELEASE(p); }
}

Mesh * Example::CreateMesh(std::string name)
{
	Mesh* mesh = new Mesh(name);
	m_MeshContainer.push_back(mesh);

	return mesh;
}

Texture * Example::CreateTexture(std::string name)
{
	Texture* texture = new Texture(name);
	m_TextureContainer.push_back(texture);

	return texture;
}

Shader * Example::CreateShader(std::string name)
{
	Shader* shader = new Shader(name);
	m_ShaderContainer.push_back(shader);

	return shader;
}

Material * Example::CreateMaterial(std::string name)
{
	Material* material = new Material(name);
	m_MaterialContainer.push_back(material);

	return material;
}

RenderNode * Example::CreateRenderNode(std::string name)
{
	RenderNode* renderNode = new RenderNode(name);
	m_RenderNodeContainer.push_back(renderNode);

	return renderNode;
}
