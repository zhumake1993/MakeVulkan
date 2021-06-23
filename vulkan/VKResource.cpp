#include "VKResource.h"
#include "VKFrame.h"
#include "GlobalSettings.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "GarbageCollector.h"

VKResource::VKResource()
	: m_GarbageCollector(GetGfxDevice().GetGarbageCollector())
{
}

VKResource::~VKResource()
{
}

bool VKResource::InUse()
{
	return GetFrameIndex() < m_FrameIndex + FrameResourcesCount;
}

void VKResource::Use()
{
	m_FrameIndex = GetFrameIndex();
}

void VKResource::Release()
{
	m_GarbageCollector->AddResource(this);
}