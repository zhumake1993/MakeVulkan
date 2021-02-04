#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "GfxTypes.h"

class GpuProgram : public NonCopyable
{
public:

	GpuProgram(GpuParameters& parameters);
	virtual ~GpuProgram();

	GpuParameters& GetGpuParameters();

	uint32_t GetUniformBufferSize(std::string name);

protected:

	GpuParameters m_GpuParameters;
};