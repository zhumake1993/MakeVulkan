#pragma once

#include "Env.h"

class VKResource
{
public:

	VKResource(uint32_t currFrameIndex);
	~VKResource();

	bool InUse(uint32_t currFrameIndex);
	void Use(uint32_t currFrameIndex);

private:

	// �ڸ�֡��ʹ��
	uint32_t m_FrameIndex = 0;
};