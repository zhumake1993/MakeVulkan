#pragma once

#include <list>
#include "NonCopyable.h"
#include "VKIncludes.h"

class VKResource;

class GarbageCollector : public NonCopyable
{
public:

	GarbageCollector();
	virtual ~GarbageCollector();

	void Update();

	uint32_t GetFrameIndex();

	void AddResource(VKResource* resource);

private:

	// 这一帧新加的Resource
	std::list<VKResource*> m_NewResources;

	// 可能还在使用中的Resource
	std::list<VKResource*> m_PendingResources;

	uint32_t m_FrameIndex = 0;
};