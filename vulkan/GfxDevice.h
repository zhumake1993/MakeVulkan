#pragma once

#include "Env.h"
#include "NonCopyable.h"

class GfxDevice : public NonCopyable
{
public:

	GfxDevice();
	~GfxDevice();

	void Init();
	void Release();
	void Update();

	void DeviceWaitIdle();
};

GfxDevice& GetGfxDevice();