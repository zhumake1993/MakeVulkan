#include "GarbageCollector.h"
#include "VKTools.h"
#include "Tools.h"
#include "VKResource.h"

namespace vk
{
	GarbageCollector::GarbageCollector()
	{
	}

	GarbageCollector::~GarbageCollector()
	{
		for (auto itr = m_Resources.begin(); itr != m_Resources.end(); itr++)
		{
			RELEASE(*itr);
		}
		m_Resources.clear();
	}

	void GarbageCollector::Add(VKResource * resource)
	{
		m_Resources.push_back(resource);
	}

	void GarbageCollector::GarbageCollect()
	{
		std::list<VKResource*>::iterator itr;
		for (itr = m_Resources.begin(); itr != m_Resources.end(); ++itr)
		{
			if (!(*itr)->InUse())
			{
				RELEASE(*itr);
			}
			else
			{
				break;
			}
		}

		m_Resources.erase(m_Resources.begin(), itr);
	}
}