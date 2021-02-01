#pragma once

#include "Env.h"
#include "NonCopyable.h"

class VKBuffer;

class VKGarbageCollector : public NonCopyable
{
public:

	VKGarbageCollector();
	~VKGarbageCollector();

	void Update(uint32_t currFrameIndex);

	void AddBuffers(VKBuffer* buffer);

private:

	std::list<VKBuffer*> m_PendingBuffers;
};