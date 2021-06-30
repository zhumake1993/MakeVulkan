#include "VKFence.h"

namespace vk
{
	FrameManager gFrameManager;

	FrameManager & GetFrameManager()
	{
		return gFrameManager;
	}

	FrameManager::FrameManager()
	{
	}

	FrameManager::~FrameManager()
	{
	}
}