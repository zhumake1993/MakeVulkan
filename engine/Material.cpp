#include "Material.h"
#include "Shader.h"
#include "GpuProgram.h"
#include "Texture.h"
#include "Tools.h"
#include "ShaderData.h"
#include "GfxDevice.h"
#include "GfxBuffer.h"

Material::Material(const std::string& name) :
	m_Name(name)
{
}

Material::~Material()
{
	RELEASE(m_ShaderData);

	if (m_UniformBuffer)
	{
		RELEASE(m_UniformBuffer);
	}
}

std::string Material::GetName()
{
	return m_Name;
}

void Material::SetShader(Shader * shader)
{
	m_Shader = shader;

	GpuParameters& gpuParameters = m_Shader->GetGpuProgram()->GetGpuParameters();

	m_ShaderData = new ShaderData();

	for (auto& uniform : gpuParameters.uniformParameters)
	{
		if (uniform.name == "PerMaterial")
		{
			m_ShaderData->SetValueParameter(uniform.valueParameters);
		}
	}

	m_ShaderData->SetTextureParameter(gpuParameters.textureParameters);

	// Buffer
	auto& device = GetGfxDevice();
	uint32_t dataSize = m_ShaderData->GetValueDataSize();
	if (dataSize > 0)
	{
		m_UniformBuffer = device.CreateBuffer(kGfxBufferUsageUniform, kGfxBufferModeHostVisible, dataSize);
		device.UpdateBuffer(m_UniformBuffer, m_ShaderData->GetValueData(), 0, dataSize);
	}
}

Shader * Material::GetShader()
{
	return m_Shader;
}

void Material::SetFloat(const std::string& name, float x)
{
	m_ShaderData->SetFloat(name, x);

	m_Dirty = true;
}

void Material::SetFloat2(const std::string& name, float x, float y)
{
	m_ShaderData->SetFloat2(name, x, y);

	m_Dirty = true;
}

void Material::SetFloat3(const std::string& name, float x, float y, float z)
{
	m_ShaderData->SetFloat3(name, x, y, z);

	m_Dirty = true;
}

void Material::SetFloat4(const std::string& name, float x, float y, float z, float w)
{
	m_ShaderData->SetFloat4(name, x, y, z, w);

	m_Dirty = true;
}

void Material::SetFloat4x4(const std::string& name, glm::mat4 & mat)
{
	m_ShaderData->SetFloat4x4(name, mat);

	m_Dirty = true;
}

void Material::SetInt(const std::string & name, int x)
{
	m_ShaderData->SetInt(name, x);

	m_Dirty = true;
}

void Material::SetTexture(const std::string& name, TextureBase * texture)
{
	m_ShaderData->SetTexture(name, texture);
}

ShaderData * Material::GetShaderData()
{
	return m_ShaderData;
}

GfxBuffer * Material::GetUniformBuffer()
{
	return m_UniformBuffer;
}

void Material::UpdateUniformBuffer()
{
	auto& device = GetGfxDevice();
	uint32_t dataSize = m_ShaderData->GetValueDataSize();
	if (dataSize > 0)
	{
		device.UpdateBuffer(m_UniformBuffer, m_ShaderData->GetValueData(), 0, dataSize);
	}
}

bool Material::IsDirty()
{
	return m_Dirty;
}

void Material::SetDirty(bool dirty)
{
	m_Dirty = dirty;
}
