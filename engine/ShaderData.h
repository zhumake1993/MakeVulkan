#pragma once

#include "Env.h"
#include "GpuProgram.h"

class Texture;

class ShaderData
{
public:

	// 支持一个uniform，多个texture
	ShaderData(GpuParameters::UniformParameter uniformParameter, std::vector<GpuParameters::TextureParameter>& textureParameters);
	ShaderData(std::vector<GpuParameters::TextureParameter>& textureParameters);
	~ShaderData();

	void SetFloat(const std::string& name, float x);
	void SetFloat2(const std::string& name, float x, float y);
	void SetFloat3(const std::string& name, float x, float y, float z);
	void SetFloat4(const std::string& name, float x, float y, float z, float w);
	void SetFloat4x4(const std::string& name, glm::mat4& mat);

	void SetTexture(const std::string& name, Texture* texture);

	void* GetDate();
	uint32_t GetDataSize();
	Texture* GetTexture(const std::string& name);

private:

	char* m_Data = nullptr;
	uint32_t m_DataSize;
	std::unordered_map<std::string, uint32_t> m_OffsetMap;
	std::unordered_map<std::string, Texture*> m_TextureMap;
};