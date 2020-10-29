#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Tools.h"
#include "DescriptorSetMgr.h"
#include "VulkanDriver.h"

Material::Material(std::string name) :
	m_Name(name)
{
}

Material::~Material()
{
	RELEASE(m_UniformData);
}

std::string Material::GetName()
{
	return m_Name;
}

void Material::SetShader(Shader * shader)
{
	m_Shader = shader;

	// uniform buffer
	RELEASE(m_UniformData);
	uint32_t uniformSize = m_Shader->GetUniformSize();
	m_UniformData = new char[uniformSize];

	if (uniformSize > 0) {
		GetVulkanDriver().CreateUniformBuffer(m_Name, uniformSize);
	}

	// texture
	auto texNum = m_Shader->GetTextureNum();
	m_Textures.resize(texNum);
	
	// DescriptorSetLayout
	DSLBindings bindings(2);
	bindings[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT };
	bindings[1] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texNum, VK_SHADER_STAGE_FRAGMENT_BIT };
	m_DescriptorSetLayout = GetVulkanDriver().CreateDescriptorSetLayout(bindings);
}

Shader * Material::GetShader()
{
	return m_Shader;
}

void Material::SetFloat4(std::string name, float x, float y, float z, float w)
{
	auto& elements = m_Shader->GetUniformElements();

	assert(elements[name].type == float4);

	float* addr = reinterpret_cast<float*>(m_UniformData + elements[name].offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
	*(addr + 2) = z;
	*(addr + 3) = w;
}

void Material::SetFloat3(std::string name, float x, float y, float z)
{
	auto& elements = m_Shader->GetUniformElements();

	assert(elements[name].type == float3);

	float* addr = reinterpret_cast<float*>(m_UniformData + elements[name].offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
	*(addr + 2) = z;
}

void Material::SetFloat2(std::string name, float x, float y)
{
	auto& elements = m_Shader->GetUniformElements();

	assert(elements[name].type == float2);

	float* addr = reinterpret_cast<float*>(m_UniformData + elements[name].offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
}

void Material::SetFloat(std::string name, float x)
{
	auto& elements = m_Shader->GetUniformElements();

	assert(elements[name].type == float1);

	float* addr = reinterpret_cast<float*>(m_UniformData + elements[name].offset);
	*(addr + 0) = x;
}

void Material::SetFloat4x4(std::string name, glm::mat4 & mat)
{
	auto& elements = m_Shader->GetUniformElements();

	assert(elements[name].type == float4x4);

	char* addr = m_UniformData + elements[name].offset;
	memcpy(addr, &mat, 4 * 4 * 4);
}

void Material::SetTextures(std::string name, Texture * texture)
{
	auto& elements = m_Shader->GetTextureElements();

	m_Textures[elements[name]] = texture;
}

char * Material::GetUniformData()
{
	return m_UniformData;
}

uint32_t Material::GetUniformDataSize()
{
	return m_Shader->GetUniformSize();
}

std::vector<Texture*>& Material::GetTextures()
{
	return m_Textures;
}

void Material::SetDirty()
{
	m_NumFramesDirty = FrameResourcesCount;
}

bool Material::IsDirty()
{
	return m_NumFramesDirty > 0;
}

void Material::Clean()
{
	if (m_NumFramesDirty > 0) {
		m_NumFramesDirty--;
	}
}