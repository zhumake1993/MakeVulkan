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

		uint64_t GetFrameIndex() { return m_FrameIndex; }

	private:

		uint64_t m_FrameIndex = 0;
	};

	FrameManager& GetFrameManager();
}