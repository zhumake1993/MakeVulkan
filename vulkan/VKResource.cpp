#include "VKResource.h"
#include "VKFrame.h"
#include "Tools.h"

VKResource::VKResource()
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
	// todo DescriptorSet �� cache������
	//ASSERT(m_FrameIndex != GetFrameIndex(), "Resource can not be used two times in a frame.");
	m_FrameIndex = GetFrameIndex();
}

size_t VKResource::Hash()
{
	return 0;
}
