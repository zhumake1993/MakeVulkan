#include "VKResource.h"
#include "Settings.h"

VKResource::VKResource(uint32_t currFrameIndex) :
	m_FrameIndex(currFrameIndex)
{
}

VKResource::~VKResource()
{
}

bool VKResource::InUse(uint32_t currFrameIndex)
{
	return currFrameIndex < m_FrameIndex + FrameResourcesCount;
}

void VKResource::Use(uint32_t currFrameIndex)
{
	m_FrameIndex = currFrameIndex;
}
