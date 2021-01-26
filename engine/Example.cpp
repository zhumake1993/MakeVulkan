#include "Example.h"
#include "Tools.h"
#include "Mesh.h"
#include "Texture.h"

Example::Example()
{
}

Example::~Example()
{
	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	//for (auto p : m_ShaderContainer) { RELEASE(p); }
	//for (auto p : m_MaterialContainer) { RELEASE(p); }
	//for (auto p : m_RenderNodeContainer) { RELEASE(p); }
}

Mesh * Example::CreateMesh()
{
	Mesh* mesh = new Mesh();
	m_MeshContainer.push_back(mesh);

	return mesh;
}

Texture * Example::CreateTexture()
{
	Texture* texture = new Texture();
	m_TextureContainer.push_back(texture);

	return texture;
}
