#pragma once

#include <list>
#include "NonCopyable.h"
#include "VKIncludes.h"

namespace vk
{
	class VKResource;

	class GarbageCollector : public NonCopyable
	{
	public:

		GarbageCollector();
		virtual ~GarbageCollector();

		void Add(VKResource* resource);
		void GarbageCollect();

	private:

		std::list<VKResource*> m_Resources;
	};
}