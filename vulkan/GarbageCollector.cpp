#include "GarbageCollector.h"
#include "VulkanTools.h"
#include "VKResource.h"

GarbageCollector::GarbageCollector()
{
}

GarbageCollector::~GarbageCollector()
{
	for (auto itr = m_NewResources.begin(); itr != m_NewResources.end(); itr++)
	{
		RELEASE(*itr);
	}
	m_NewResources.clear();

	for (auto itr = m_PendingResources.begin(); itr != m_PendingResources.end(); itr++)
	{
		RELEASE(*itr);
	}
	m_PendingResources.clear();
}

void GarbageCollector::Update()
{
	for (auto itr = m_PendingResources.begin(); itr != m_PendingResources.end();)
	{
		if ((*itr)->InUse())
		{
			itr++;
		}
		else
		{
			RELEASE(*itr);
			itr = m_PendingResources.erase(itr);
		}
	}

	// 新的Resource放在list前部
	m_PendingResources.splice(m_PendingResources.begin(), m_NewResources);
	m_NewResources.clear();

	m_FrameIndex++;
}

uint32_t GarbageCollector::GetFrameIndex()
{
	return m_FrameIndex;
}

void GarbageCollector::AddResource(VKResource * resource)
{
	m_NewResources.push_back(resource);
}