#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "GfxTypes.h"

class GpuProgram : public NonCopyable
{
public:

	GpuProgram(GpuParameters& parameters);
	~GpuProgram();

	GpuParameters& GetGpuParameters();

	uint32_t GetUniformBufferSize(std::string name);

protected:

	GpuParameters m_GpuParameters;
};