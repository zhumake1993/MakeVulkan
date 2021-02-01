#include "VKGarbageCollector.h"
#include "VKBuffer.h"

VKGarbageCollector::VKGarbageCollector()
{
}

VKGarbageCollector::~VKGarbageCollector()
{
}

void VKGarbageCollector::Update(uint32_t currFrameIndex)
{
	for (auto itr = m_PendingBuffers.begin(); itr != m_PendingBuffers.end(); itr++)
	{
		if ((*itr)->InUse(currFrameIndex))
		{
			itr++;
		}
		else
		{
			itr = m_PendingBuffers.erase(itr);
		}
	}
}

void VKGarbageCollector::AddBuffers(VKBuffer * buffer)
{
	m_PendingBuffers.push_back(buffer);
}
