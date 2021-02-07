#include "VKResource.h"

VKResource::VKResource()
{
}

VKResource::~VKResource()
{
}

bool VKResource::InUse(int currFrameIndex)
{
	return currFrameIndex < m_FrameIndex + FrameResourcesCount;
}

void VKResource::Use(int currFrameIndex)
{
	m_FrameIndex = currFrameIndex;
}