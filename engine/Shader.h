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

	// 成熟的做法是使用反射，这里手动设置
	// 假设数据是紧密pack的（满足对齐要求），这样方便处理
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