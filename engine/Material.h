#pragma once

#include "Common.h"
#include "VKTypes.h"

class Texture;
class Shader;

class Material
{

public:

	Material(uint32_t index);
	~Material();

	void SetDUBIndex(uint32_t index);
	uint32_t GetDUBIndex();

	void SetDiffuseAlbedo(float r, float g, float b, float a);
	void SetDiffuseAlbedo(glm::vec4& diffuseAlbedo);
	glm::vec4 GetDiffuseAlbedo();

	void SetFresnelR0(float r, float g, float b);
	void SetFresnelR0(glm::vec3& fresnelR0);
	glm::vec3 GetFresnelR0();

	void SetRoughness(float roughness);
	float GetRoughness();

	void SetMatTransform(glm::mat4& matTransform);
	glm::mat4 GetMatTransform();

	void SetShader(Shader* shader);
	Shader* GetShader();

	void SetTextures(const std::vector<Texture*> textures);

	void SetDirty();
	bool IsDirty();
	void Clean();

private:

	//

public:

	//

private:

	uint32_t m_DUBIndex;

	glm::vec4 m_DiffuseAlbedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 m_FresnelR0 = glm::vec3(0.0f, 0.0f, 0.0);
	float m_Roughness = 0.0f;
	glm::mat4 m_MatTransform = glm::mat4(1.0f);

	Shader* m_Shader;

	std::vector<Texture*> m_Textures;

	uint32_t m_NumFramesDirty = FrameResourcesCount;
};