#include "ShaderData.h"
#include "Tools.h"

ShaderData::ShaderData()
{
}

ShaderData::~ShaderData()
{
	RELEASE(m_ValueData);
	RELEASE(m_SCData);
}

void ShaderData::SetValueParameter(std::vector<GpuParameters::ValueParameter>& valueParameters)
{
	uint32_t offset = 0;
	for (auto& vp : valueParameters)
	{
		m_ValueOffsetMap[vp.name] = offset;
		offset += ShaderDataTypeToSize(vp.type);
	}

	m_ValueData = new char[offset];
	memset(m_ValueData, 0, offset);

	m_ValueDataSize = offset;
}

void ShaderData::SetFloat(const std::string& name, float x)
{
	uint32_t offset = m_ValueOffsetMap[name];

	float* addr = reinterpret_cast<float*>(m_ValueData + offset);
	*(addr + 0) = x;
}

void ShaderData::SetFloat2(const std::string& name, float x, float y)
{
	uint32_t offset = m_ValueOffsetMap[name];

	float* addr = reinterpret_cast<float*>(m_ValueData + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
}

void ShaderData::SetFloat3(const std::string& name, float x, float y, float z)
{
	uint32_t offset = m_ValueOffsetMap[name];

	float* addr = reinterpret_cast<float*>(m_ValueData + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
	*(addr + 2) = z;
}

void ShaderData::SetFloat4(const std::string& name, float x, float y, float z, float w)
{
	uint32_t offset = m_ValueOffsetMap[name];

	float* addr = reinterpret_cast<float*>(m_ValueData + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
	*(addr + 2) = z;
	*(addr + 3) = w;
}

void ShaderData::SetFloat4x4(const std::string& name, glm::mat4 & mat)
{
	uint32_t offset = m_ValueOffsetMap[name];

	char* addr = m_ValueData + offset;
	memcpy(addr, &mat, sizeof(glm::mat4));
}

void ShaderData::SetInt(const std::string & name, int x)
{
	uint32_t offset = m_ValueOffsetMap[name];

	int* addr = reinterpret_cast<int*>(m_ValueData + offset);
	*(addr + 0) = x;
}

void * ShaderData::GetValueData()
{
	return m_ValueData;
}

uint32_t ShaderData::GetValueDataSize()
{
	return m_ValueDataSize;
}

void ShaderData::SetTextureParameter(std::vector<GpuParameters::TextureParameter>& textureParameters)
{
	for (auto& texture : textureParameters)
	{
		m_TextureMap[texture.name] = nullptr;
	}
}

void ShaderData::SetTexture(const std::string& name, TextureBase * texture)
{
	if (m_TextureMap.find(name) == m_TextureMap.end())
	{
		LOGE("does not contain texture %s", name.c_str());
	}

	m_TextureMap[name] = texture;
}

TextureBase * ShaderData::GetTexture(const std::string& name)
{
	if (m_TextureMap.find(name) == m_TextureMap.end())
	{
		LOGE("does not contain texture %s", name.c_str());
	}

	return m_TextureMap[name];
}

void ShaderData::SetSpecializationConstantParameter(std::vector<GpuParameters::SpecializationConstantParameter>& scParameters)
{
	uint32_t offset = 0;
	for (auto& sc : scParameters)
	{
		m_SCOffsetMap[sc.id] = offset;
		offset += ShaderDataTypeToSize(sc.type);
	}

	m_SCData = new char[offset];
	memset(m_SCData, 0, offset);

	m_SCDataSize = offset;
}

void ShaderData::SetSCFloat(int id, float x)
{
	uint32_t offset = m_SCOffsetMap[id];

	float* addr = reinterpret_cast<float*>(m_SCData + offset);
	*(addr + 0) = x;
}

void ShaderData::SetSCInt(int id, int x)
{
	uint32_t offset = m_SCOffsetMap[id];

	int* addr = reinterpret_cast<int*>(m_SCData + offset);
	*(addr + 0) = x;
}

void * ShaderData::GetSCData()
{
	return m_SCData;
}
