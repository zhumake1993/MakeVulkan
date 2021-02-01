#include "VKGarbageCollector.h"
#include "VKBuffer.h"
#include "VKDescriptorSet.h"

VKGarbageCollector::VKGarbageCollector(VkDevice vkDevice, VkDescriptorPool pool) :
	m_Device(vkDevice),
	m_DescriptorPool(pool)
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

	for (auto itr = m_PendingDescriptorSets.begin(); itr != m_PendingDescriptorSets.end(); itr++)
	{
		if ((*itr)->InUse(currFrameIndex))
		{
			itr++;
		}
		else
		{
			vkFreeDescriptorSets(m_Device, m_DescriptorPool, 1, &(*itr)->descriptorSet);
			itr = m_PendingDescriptorSets.erase(itr);
		}
	}
}

void VKGarbageCollector::AddBuffer(VKBuffer * buffer)
{
	m_PendingBuffers.push_back(buffer);
}

void VKGarbageCollector::AddDescriptorSet(VKDescriptorSet * descriptorSet)
{
	m_PendingDescriptorSets.push_back(descriptorSet);
}
