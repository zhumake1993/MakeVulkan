#include "ShaderData.h"
#include "Tools.h"

uint32_t UniformDataTypeToSize(GpuParameters::UniformDataType type)
{
	switch (type)
	{
		case GpuParameters::kUniformDataFloat1:
			return 1 * 4;
			break;
		case GpuParameters::kUniformDataFloat2:
			return 2 * 4;
			break;
		case GpuParameters::kUniformDataFloat3:
			return 3 * 4;
			break;
		case GpuParameters::kUniformDataFloat4:
			return 4 * 4;
			break;
		case GpuParameters::kUniformDataFloat4x4:
			return 4 * 4 * 4;
			break;
		default:
			LOG("wrong UniformDataType");
			EXIT;
			return 0;
	}
}

ShaderData::ShaderData(GpuParameters::UniformParameter& uniformParameter, std::vector<GpuParameters::TextureParameter>& textureParameters)
{
	CreateFromUniformParameter(uniformParameter);
	CreateFromTextureParameter(textureParameters);
}

ShaderData::ShaderData(GpuParameters::UniformParameter & uniformParameter)
{
	CreateFromUniformParameter(uniformParameter);
}

ShaderData::ShaderData(std::vector<GpuParameters::TextureParameter>& textureParameters)
{
	CreateFromTextureParameter(textureParameters);
}

ShaderData::~ShaderData()
{
	RELEASE(m_Data);
}

void ShaderData::SetFloat(const std::string& name, float x)
{
	uint32_t offset = m_OffsetMap[name];

	float* addr = reinterpret_cast<float*>(m_Data + offset);
	*(addr + 0) = x;
}

void ShaderData::SetFloat2(const std::string& name, float x, float y)
{
	uint32_t offset = m_OffsetMap[name];

	float* addr = reinterpret_cast<float*>(m_Data + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
}

void ShaderData::SetFloat3(const std::string& name, float x, float y, float z)
{
	uint32_t offset = m_OffsetMap[name];

	float* addr = reinterpret_cast<float*>(m_Data + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
	*(addr + 2) = z;
}

void ShaderData::SetFloat4(const std::string& name, float x, float y, float z, float w)
{
	uint32_t offset = m_OffsetMap[name];

	float* addr = reinterpret_cast<float*>(m_Data + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
	*(addr + 2) = z;
	*(addr + 3) = w;
}

void ShaderData::SetFloat4x4(const std::string& name, glm::mat4 & mat)
{
	uint32_t offset = m_OffsetMap[name];

	char* addr = m_Data + offset;
	memcpy(addr, &mat, sizeof(glm::mat4));
}

void ShaderData::SetTexture(const std::string& name, Texture * texture)
{
	if (m_TextureMap.find(name) == m_TextureMap.end())
	{
		LOGE("material does not contain texture %s", name.c_str());
	}

	m_TextureMap[name] = texture;
}

void * ShaderData::GetDate()
{
	return m_Data;
}

uint32_t ShaderData::GetDataSize()
{
	return m_DataSize;
}

Texture * ShaderData::GetTexture(const std::string& name)
{
	if (m_TextureMap.find(name) == m_TextureMap.end())
	{
		LOGE("material does not contain texture %s", name.c_str());
	}

	return m_TextureMap[name];
}

void ShaderData::CreateFromUniformParameter(GpuParameters::UniformParameter & uniformParameter)
{
	uint32_t offset = 0;
	for (auto& vp : uniformParameter.valueParameters)
	{
		m_OffsetMap[vp.name] = offset;
		offset += UniformDataTypeToSize(vp.type);
	}

	m_Data = new char[offset];
	memset(m_Data, 0, offset);

	m_DataSize = offset;
}

void ShaderData::CreateFromTextureParameter(std::vector<GpuParameters::TextureParameter>& textureParameters)
{
	for (auto& texture : textureParameters)
	{
		m_TextureMap[texture.name] = nullptr;
	}
}
