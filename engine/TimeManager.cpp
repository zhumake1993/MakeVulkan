#include "TimeManager.h"
#include "Tools.h"

//TimeManager* gTimeManager;
//
//void CreateTimeManager()
//{
//	gTimeManager = new TimeManager();
//}
//
//TimeManager & GetTimeManager()
//{
//	return *gTimeManager;
//}
//
//void ReleaseTimeManager()
//{
//	RELEASE(gTimeManager);
//}

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

int TimeManager::GetFrameIndex()
{
	return m_FrameIndex;
}

float TimeManager::GetDeltaTime()
{
	return m_DeltaTime;
}

float TimeManager::GetFPS()
{
	return m_FPS;
}

void TimeManager::Update()
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
	if (m_AccumulateTime >= 1.0f) {
		m_FPS = m_AccumulateCounter / m_AccumulateTime;
		m_AccumulateCounter = 0;
		m_AccumulateTime = 0;
	}
}
