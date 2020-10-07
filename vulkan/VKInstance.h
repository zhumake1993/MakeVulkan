#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKInstance : public NonCopyable
{
	VKInstance();
	~VKInstance();

	VkInstance instance = VK_NULL_HANDLE;
};