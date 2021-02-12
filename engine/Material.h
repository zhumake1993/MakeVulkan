#pragma once

#include "Env.h"
#include "GpuProgram.h"

class Shader;
class Texture;
class ShaderData;
class Buffer;

class Material
{

public:

	Material(const std::string& name);
	virtual ~Material();

	std::string GetName();

	void SetShader(Shader* shader);
	Shader* GetShader();

	void SetFloat(const std::string& name, float x);
	void SetFloat2(const std::string& name, float x, float y);
	void SetFloat3(const std::string& name, float x, float y, float z);
	void SetFloat4(const std::string& name, float x, float y, float z, float w);
	void SetFloat4x4(const std::string& name, glm::mat4& mat);

	void SetTexture(const std::string& name, Texture* texture);

	ShaderData* GetShaderData();

	Buffer* GetUniformBuffer();
	void UpdateUniformBuffer();

	bool IsDirty();
	void SetDirty(bool dirty);

public:

	//

private:

	std::string m_Name;

	Shader* m_Shader = nullptr;

	ShaderData* m_ShaderData = nullptr;

	Buffer* m_UniformBuffer = nullptr;

	bool m_Dirty = true;
};