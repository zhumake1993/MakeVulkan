#pragma once

#include "Env.h"
#include "GpuProgram.h"

class Shader;
class Texture;

class Material
{

public:

	Material(std::string name);
	virtual ~Material();

	std::string GetName();

	void SetShader(Shader* shader);
	Shader* GetShader();

	void SetFloat(std::string name, float x);
	void SetFloat2(std::string name, float x, float y);
	void SetFloat3(std::string name, float x, float y, float z);
	void SetFloat4(std::string name, float x, float y, float z, float w);
	void SetFloat4x4(std::string name, glm::mat4& mat);

	void SetTextures(std::string name, Texture* texture);

	//void SetVKPipeline(VKPipeline* vkPipeline);
	//VKPipeline* GetVKPipeline();

	//char* GetUniformData();
	//uint32_t GetUniformDataSize();

	std::vector<Texture*>& GetTextures();

	//void SetDirty();
	//bool IsDirty();
	//void Clean();

	// todo
	//VkDescriptorSetLayout GetVkDescriptorSetLayout() { return m_DescriptorSetLayout; }

private:

	uint32_t GetUniformBufferDataOffset(std::string name, UniformDataType type);

public:

	//

private:

	std::string m_Name;

	Shader* m_Shader = nullptr;

	char* m_UniformBufferData = nullptr;

	std::vector<Texture*> m_Textures;

	//VKPipeline* m_VKPipeline = VK_NULL_HANDLE;

	//VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

	//uint32_t m_NumFramesDirty = FrameResourcesCount;
};