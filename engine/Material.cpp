#include "Material.h"
#include "Texture.h"
#include "Shader.h"

Material::Material(uint32_t index) :
	m_DUBIndex(index)
{
	m_DiffuseAlbedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

Material::~Material()
{
}

void Material::SetDUBIndex(uint32_t index)
{
	m_DUBIndex = index;
}

uint32_t Material::GetDUBIndex()
{
	return m_DUBIndex;
}

void Material::SetDiffuseAlbedo(float r, float g, float b, float a)
{
	m_DiffuseAlbedo = glm::vec4(r, g, b, a);
}

void Material::SetDiffuseAlbedo(glm::vec4 & diffuseAlbedo)
{
	m_DiffuseAlbedo = diffuseAlbedo;
}

glm::vec4 Material::GetDiffuseAlbedo()
{
	return m_DiffuseAlbedo;
}

void Material::SetFresnelR0(float r, float g, float b)
{
	m_FresnelR0 = glm::vec3(r, g, b);
}

void Material::SetFresnelR0(glm::vec3 & fresnelR0)
{
	m_FresnelR0 = fresnelR0;
}

glm::vec3 Material::GetFresnelR0()
{
	return m_FresnelR0;
}

void Material::SetRoughness(float roughness)
{
	m_Roughness = roughness;
}

float Material::GetRoughness()
{
	return m_Roughness;
}

void Material::SetMatTransform(glm::mat4 & matTransform)
{
	m_MatTransform = matTransform;
}

glm::mat4 Material::GetMatTransform()
{
	return m_MatTransform;
}

void Material::SetShader(Shader * shader)
{
	m_Shader = shader;
}

Shader * Material::GetShader()
{
	return m_Shader;
}

void Material::SetTextures(const std::vector<Texture*> textures)
{
	m_Textures = textures;
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