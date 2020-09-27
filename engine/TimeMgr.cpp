#include "TimeMgr.h"

TimeMgr::TimeMgr()
{
}

TimeMgr::~TimeMgr()
{
}

uint32_t TimeMgr::GetFrameIndex()
{
	return m_FrameIndex;
}

float TimeMgr::GetDeltaTime()
{
	return m_DeltaTime;
}

float TimeMgr::GetFPS()
{
	return m_FPS;
}

void TimeMgr::Tick()
{
	// frame index
	m_FrameIndex++;

	// delta time
	auto timestamp = std::chrono::high_resolution_clock::now();
	m_DeltaTime = (timestamp - m_LastTimestamp).count() / 1000000000.0f;
	m_LastTimestamp = timestamp;

	// fps
	m_AccumulateCounter++;
	m_AccumulateTime += m_DeltaTime;
	if (m_AccumulateTime >= 0.5f) {
		m_FPS = m_AccumulateCounter / m_AccumulateTime;
		m_AccumulateCounter = 0;
		m_AccumulateTime = 0;
	}
}
