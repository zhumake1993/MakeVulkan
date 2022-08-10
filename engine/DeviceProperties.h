#pragma once

#include "NonCopyable.h"
#include "mkString.h"

struct DeviceProperties : public NonCopyable
{
	mkString deviceName;

	uint64_t minUniformBufferOffsetAlignment;
};

DeviceProperties& GetDeviceProperties();