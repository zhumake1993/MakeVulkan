#pragma once

#include "GpuProgram.h"
#include "GLMIncludes.h"
#include "mkHashMap.h"

class TextureBase;

class ShaderData
{
public:

	ShaderData();
	~ShaderData();

	// ValueParameter

	void SetValueParameter(std::vector<GpuParameters::ValueParameter>& valueParameters);

	void SetFloat(const mkString& name, float x);
	void SetFloat2(const mkString& name, float x, float y);
	void SetFloat3(const mkString& name, float x, float y, float z);
	void SetFloat4(const mkString& name, float x, float y, float z, float w);
	void SetFloat4x4(const mkString& name, glm::mat4& mat);

	void SetInt(const mkString& name, int x);

	void* GetValueData();
	uint32_t GetValueDataSize();

	// TextureParameter

	void SetTextureParameter(std::vector<GpuParameters::TextureParameter>& textureParameters);

	void SetTexture(const mkString& name, TextureBase* texture);

	TextureBase* GetTexture(const mkString& name);

	// SpecializationConstant

	void SetSpecializationConstantParameter(std::vector<GpuParameters::SpecializationConstantParameter>& scParameters);

	void SetSCFloat(int id, float x);
	void SetSCInt(int id, int x);

	void* GetSCData();

private:

	// ValueParameter
	char* m_ValueData = nullptr;
	uint32_t m_ValueDataSize = 0;
	mkHashMap<mkString, uint32_t> m_ValueOffsetMap;

	// TextureParameter
	mkHashMap<mkString, TextureBase*> m_TextureMap;

	// SpecializationConstant
	char* m_SCData = nullptr;
	uint32_t m_SCDataSize = 0;
	mkHashMap<int, uint32_t> m_SCOffsetMap;
};