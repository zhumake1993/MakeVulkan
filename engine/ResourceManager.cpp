#include "ResourceManager.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "RenderNode.h"

ResourceManager gResourceManager;

ResourceManager & GetResourceManager()
{
	return gResourceManager;
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	for (auto p : m_ShaderContainer) { RELEASE(p); }
	for (auto p : m_MaterialContainer) { RELEASE(p); }
	for (auto p : m_RenderNodeContainer) { RELEASE(p); }

	for (auto p : m_TempAttachments) { RELEASE(p); }
}

Mesh * ResourceManager::CreateMesh(const std::string& name)
{
	Mesh* mesh = new Mesh(name);
	m_MeshContainer.push_back(mesh);

	return mesh;
}

Texture * ResourceManager::CreateTexture(const std::string& name)
{
	Texture* texture = new Texture(name);
	m_TextureContainer.push_back(texture);

	return texture;
}

Shader * ResourceManager::CreateShader(const std::string& name)
{
	Shader* shader = new Shader(name);
	m_ShaderContainer.push_back(shader);

	return shader;
}

Material * ResourceManager::CreateMaterial(const std::string& name)
{
	Material* material = new Material(name);
	m_MaterialContainer.push_back(material);

	return material;
}

RenderNode * ResourceManager::CreateRenderNode(const std::string& name)
{
	RenderNode* renderNode = new RenderNode(name);
	m_RenderNodeContainer.push_back(renderNode);

	return renderNode;
}

Attachment * ResourceManager::CreateAttachment(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height)
{
	if (imageTypeMask & kImageSwapChainBit)
	{
		format = GetGfxDevice().GetSwapChainFormat();

		VkExtent2D extent = GetGfxDevice().GetSwapChainExtent();
		width = extent.width;
		height = extent.height;
	}

	return new Attachment(imageTypeMask, format, width, height);
}

Attachment * ResourceManager::CreateTempAttachment(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height)
{
	Attachment* attachment = CreateAttachment(imageTypeMask, format, width, height);
	m_TempAttachments.push_back(attachment);
	return attachment;
}

void ResourceManager::ReleaseTempAttachment()
{
	for (auto p : m_TempAttachments) { RELEASE(p); }
	m_TempAttachments.clear();
}