#include "GfxDevice.h"

GfxDevice* gfxDevice;

GfxDevice& GetGfxDevice()
{
	return *gfxDevice;
}

GfxDevice::GfxDevice()
{
}

GfxDevice::~GfxDevice()
{
}

void GfxDevice::Init()
{
}

void GfxDevice::Release()
{
}

void GfxDevice::Update()
{
}

void GfxDevice::DeviceWaitIdle()
{
}
