#include "VKGarbageCollector.h"
#include "VulkanTools.h"
#include "VKBuffer.h"

VKGarbageCollector::VKGarbageCollector(VkDevice vkDevice) :
	m_Device(vkDevice)
	//m_DescriptorPool(pool)
{
}

VKGarbageCollector::~VKGarbageCollector()
{
}

void VKGarbageCollector::Update(uint32_t currFrameIndex)
{
	for (auto itr = m_PendingBuffers.begin(); itr != m_PendingBuffers.end();)
	{
		if ((*itr)->InUse(currFrameIndex))
		{
			itr++;
		}
		else
		{
			RELEASE(*itr);
			itr = m_PendingBuffers.erase(itr);
		}
	}

	
}

void VKGarbageCollector::AddBuffer(VKBuffer * buffer)
{
	m_PendingBuffers.push_back(buffer);
}

//void VKGarbageCollector::AddDescriptorSet(VKDescriptorSet * descriptorSet)
//{
//	m_PendingDescriptorSets.push_back(descriptorSet);
//}
