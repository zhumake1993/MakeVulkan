#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"

namespace vk
{
	class FrameManager :NonCopyable
	{
	public:

		FrameManager();
		~FrameManager();

		void IncreaseFrameIndex() { ++m_FrameIndex; }

		int GetFrameIndex() { return m_FrameIndex; }

	private:

		int m_FrameIndex = 0;
	};

	FrameManager& GetFrameManager();
}