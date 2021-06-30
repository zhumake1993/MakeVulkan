#pragma once

#include "NamedObject.h"
#include "GpuProgram.h"
#include "GLMIncludes.h"

class Shader;
class TextureBase;
class ShaderData;
class GfxBuffer;

class Material : public NamedObject
{

public:

	Material(const mkString& name);
	virtual ~Material();

	mkString GetName();

	void SetShader(Shader* shader);
	Shader* GetShader();

	void SetFloat(const mkString& name, float x);
	void SetFloat2(const mkString& name, float x, float y);
	void SetFloat3(const mkString& name, float x, float y, float z);
	void SetFloat4(const mkString& name, float x, float y, float z, float w);
	void SetFloat4x4(const mkString& name, glm::mat4& mat);

	void SetInt(const mkString& name, int x);

	void SetTexture(const mkString& name, TextureBase* texture);

	ShaderData* GetShaderData();

	GfxBuffer* GetUniformBuffer();
	void UpdateUniformBuffer();

	bool IsDirty();
	void SetDirty(bool dirty);

public:

	//

private:

	Shader* m_Shader = nullptr;

	ShaderData* m_ShaderData = nullptr;

	GfxBuffer* m_UniformBuffer = nullptr;

	bool m_Dirty = true;
};