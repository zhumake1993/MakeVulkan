#include "VKResource.h"
#include "VKFence.h"
#include "GlobalSettings.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "GarbageCollector.h"

namespace vk
{
	VKResource::VKResource()
		: m_GarbageCollector(GetGfxDevice().GetGarbageCollector())
	{
	}

	VKResource::~VKResource()
	{
	}

	bool VKResource::InUse()
	{
		return GetFrameManager().GetFrameIndex() < m_FrameIndex + FrameResourcesCount;
	}

	void VKResource::Use()
	{
		m_FrameIndex = GetFrameManager().GetFrameIndex();
	}

	void VKResource::Release()
	{
		m_GarbageCollector->Add(this);
	}
}