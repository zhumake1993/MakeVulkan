#pragma once

#include "mkList.h"
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

		mkList<VKResource*> m_Resources;
	};
}