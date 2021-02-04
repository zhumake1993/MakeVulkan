#pragma once

#include "Env.h"
#include "NonCopyable.h"

struct VKInstance : public NonCopyable
{
	VKInstance();
	virtual ~VKInstance();

	VkInstance instance = VK_NULL_HANDLE;
};