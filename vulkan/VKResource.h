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

	// �ڸ�֡��ʹ��
	int m_FrameIndex = -FrameResourcesCount;
};