#include "Shader.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "GpuProgram.h"
#include "ShaderData.h"

Shader::Shader(const std::string& name) :
	m_Name(name)
{
}

Shader::~Shader()
{
	m_VertCode.clear();
	m_FragCode.clear();
	RELEASE(m_GpuProgram);
	RELEASE(m_SpecializationShaderData);
}

void Shader::LoadSPV(const std::string & vertFilename, const std::string & fragFilename)
{
	m_VertCode = GetBinaryFileContents(vertFilename);
	assert(m_VertCode.size() > 0);
	
	m_FragCode = GetBinaryFileContents(fragFilename);
	assert(m_FragCode.size() > 0);
}

void Shader::CreateGpuProgram(GpuParameters & parameters)
{
	auto& device = GetGfxDevice();

	m_GpuProgram = device.CreateGpuProgram(parameters, m_VertCode, m_FragCode);

	m_SpecializationShaderData = new ShaderData();
	m_SpecializationShaderData->SetSpecializationConstantParameter(parameters.SCParameters);

	m_VertCode.clear();
	m_FragCode.clear();
}

GpuProgram * Shader::GetGpuProgram()
{
	return m_GpuProgram;
}

void Shader::SetRenderState(RenderState & renderState)
{
	m_RenderState = renderState;
}

RenderState * Shader::GetRenderState()
{
	return &m_RenderState;
}

void Shader::SetSCFloat(int id, float x)
{
	m_SpecializationShaderData->SetSCFloat(id, x);
}

void Shader::SetSCInt(int id, int x)
{
	m_SpecializationShaderData->SetSCInt(id, x);
}

void * Shader::GetSpecializationData()
{
	return m_SpecializationShaderData->GetSCData();
}