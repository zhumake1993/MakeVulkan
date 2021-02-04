#include "GpuProgram.h"

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

uint32_t GpuProgram::GetUniformBufferSize(std::string name)
{
	for (auto& layout : m_GpuParameters.uniformBufferLayouts)
	{
		if (layout.name == name)
		{
			return layout.GetSize();
		}
	}

	//todo
	return 256;
}
