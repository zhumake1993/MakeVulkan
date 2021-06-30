#pragma once

#include "GpuProgram.h"
#include "GfxTypes.h"
#include "mkString.h"
#include "mkVector.h"

class GpuProgram;
class ShaderData;

class Shader
{

public:

	Shader(const mkString& name);
	virtual ~Shader();

	void LoadSPV(const mkString& vertFilename, const mkString& fragFilename);

	// �����������ʹ�÷��䣬�����ֶ�����
	// ���������ǽ���pack�ģ��������Ҫ�󣩣��������㴦��
	void CreateGpuProgram(GpuParameters& parameters);
	GpuProgram* GetGpuProgram();

	void SetRenderState(RenderState& renderState);
	RenderState* GetRenderState();

	void SetSCFloat(int id, float x);
	void SetSCInt(int id, int x);
	void* GetSpecializationData();

public:

	//

private:

	mkString m_Name;

	mkVector<char> m_VertCode;
	mkVector<char> m_FragCode;

	GpuProgram* m_GpuProgram;

	RenderState m_RenderState;

	ShaderData* m_SpecializationShaderData;
};