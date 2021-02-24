#pragma once

#include "Env.h"
#include "GpuProgram.h"

class Texture;

class ShaderData
{
public:

	ShaderData();
	~ShaderData();

	// ValueParameter

	void SetValueParameter(std::vector<GpuParameters::ValueParameter>& valueParameters);

	void SetFloat(const std::string& name, float x);
	void SetFloat2(const std::string& name, float x, float y);
	void SetFloat3(const std::string& name, float x, float y, float z);
	void SetFloat4(const std::string& name, float x, float y, float z, float w);
	void SetFloat4x4(const std::string& name, glm::mat4& mat);

	void SetInt(const std::string& name, int x);

	void* GetValueData();
	uint32_t GetValueDataSize();

	// TextureParameter

	void SetTextureParameter(std::vector<GpuParameters::TextureParameter>& textureParameters);

	void SetTexture(const std::string& name, Texture* texture);

	Texture* GetTexture(const std::string& name);

	// SpecializationConstant

	void SetSpecializationConstantParameter(std::vector<GpuParameters::SpecializationConstantParameter>& scParameters);

	void SetSCFloat(int id, float x);
	void SetSCInt(int id, int x);

	void* GetSCData();

private:

	// ValueParameter
	char* m_ValueData = nullptr;
	uint32_t m_ValueDataSize = 0;
	std::unordered_map<std::string, uint32_t> m_ValueOffsetMap;

	// TextureParameter
	std::unordered_map<std::string, Texture*> m_TextureMap;

	// SpecializationConstant
	char* m_SCData = nullptr;
	uint32_t m_SCDataSize = 0;
	std::unordered_map<int, uint32_t> m_SCOffsetMap;
};