#pragma once

#include "NonCopyable.h"
#include <vector>
#include "VKIncludes.h"//todo

class Mesh;
class Texture;
class Shader;
class Material;
class RenderNode;
class Attachment;

class ResourceManager : public NonCopyable
{
public:

	ResourceManager();
	virtual ~ResourceManager();

	Mesh* CreateMesh(const std::string& name);
	Texture* CreateTexture(const std::string& name);
	Shader* CreateShader(const std::string& name);
	Material* CreateMaterial(const std::string& name);
	RenderNode* CreateRenderNode(const std::string& name);

	Attachment* CreateAttachment(int imageTypeMask, VkFormat format = VK_FORMAT_UNDEFINED, uint32_t width = 0, uint32_t height = 0); // kImageSwapChain类型不需要其他参数
	Attachment* CreateTempAttachment(int imageTypeMask, VkFormat format = VK_FORMAT_UNDEFINED, uint32_t width = 0, uint32_t height = 0);

	//todo
	void ReleaseTempAttachment();

private:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;

	std::vector<Attachment*> m_TempAttachments;
};

ResourceManager& GetResourceManager();