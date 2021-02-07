#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "Settings.h"

class VKResource : public NonCopyable
{
public:

	VKResource();
	virtual ~VKResource();

	bool InUse(int currFrameIndex);
	void Use(int currFrameIndex);

private:

	// 在该帧被使用
	int m_FrameIndex = -FrameResourcesCount;
};