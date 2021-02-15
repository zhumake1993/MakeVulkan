#pragma once

#include "Env.h"
#include "GpuProgram.h"
#include "GfxTypes.h"

class GpuProgram;
class ShaderData;

class Shader
{

public:

	Shader(const std::string& name);
	virtual ~Shader();

	void LoadSPV(const std::string& vertFilename, const std::string& fragFilename);

	// �����������ʹ�÷��䣬�����ֶ�����
	// ���������ǽ���pack�ģ��������Ҫ�󣩣��������㴦��
	void CreateGpuProgram(GpuParameters& parameters);
	GpuProgram* GetGpuProgram();

	void SetRenderState(RenderState& renderState);
	RenderState& GetRenderState();

	void SetSCFloat(int id, float x);
	void SetSCInt(int id, int x);
	void* GetSpecializationData();

public:

	//

private:

	std::string m_Name;

	std::vector<char> m_VertCode;
	std::vector<char> m_FragCode;

	GpuProgram* m_GpuProgram;

	RenderState m_RenderState;

	ShaderData* m_SpecializationShaderData;
};