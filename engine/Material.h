#pragma once

#include "Common.h"
#include "VKTypes.h"

class Texture;
class Shader;

class Material
{

public:

	Material();
	~Material();

	void SetShader(Shader* shader);
	Shader* GetShader();

	void SetTextures(const std::vector<Texture*> textures);

private:

	//

public:

	//

private:

	Shader* m_Shader;

	std::vector<Texture*> m_Textures;
};