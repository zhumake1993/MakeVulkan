#include "GpuProgram.h"
#include "Tools.h"

GpuProgram::GpuProgram(GpuParameters& parameters) :
	m_GpuParameters(parameters)
{
}

GpuProgram::~GpuProgram()
{
}

GpuParameters & GpuProgram::GetGpuParameters()
{
	return m_GpuParameters;
}