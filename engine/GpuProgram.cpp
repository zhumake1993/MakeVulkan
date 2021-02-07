#include "GpuProgram.h"
#include "Tools.h"

std::string UniformDataTypeToString(UniformDataType type)
{
	switch (type)
	{
	case kUniformDataTypeFloat1:
		return "kUniformDataTypeFloat1";
		break;
	case kUniformDataTypeFloat2:
		return "kUniformDataTypeFloat2";
		break;
	case kUniformDataTypeFloat3:
		return "kUniformDataTypeFloat3";
		break;
	case kUniformDataTypeFloat4:
		return "kUniformDataTypeFloat4";
		break;
	case kUniformDataTypeFloat4x4:
		return "kUniformDataTypeFloat4x4";
		break;
	default:
		LOG("wrong UniformDataType");
		EXIT;
		return 0;
	}
}

uint32_t UniformDataTypeToSize(UniformDataType type)
{
	switch (type)
	{
	case kUniformDataTypeFloat1:
		return 1 * 4;
		break;
	case kUniformDataTypeFloat2:
		return 2 * 4;
		break;
	case kUniformDataTypeFloat3:
		return 3 * 4;
		break;
	case kUniformDataTypeFloat4:
		return 4 * 4;
		break;
	case kUniformDataTypeFloat4x4:
		return 4 * 4 * 4;
		break;
	default:
		LOG("wrong UniformDataType");
		EXIT;
		return 0;
	}
}

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
