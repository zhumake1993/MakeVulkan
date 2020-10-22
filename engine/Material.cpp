#include "Material.h"
#include "Texture.h"
#include "Shader.h"

Material::Material()
{
}

Material::~Material()
{
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
