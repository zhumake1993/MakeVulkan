#pragma once

#include "Common.h" 

class TimeMgr
{

public:

	TimeMgr();
	~TimeMgr();

	uint32_t GetFrameIndex();
	float GetDeltaTime();
	float GetFPS();
	void Tick();

private:

	//

public:

	//

private:

	uint32_t m_FrameIndex = 0;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_LastTimestamp = std::chrono::high_resolution_clock::now();
	float m_DeltaTime = 0;

	uint32_t m_AccumulateCounter = 0;
	float m_AccumulateTime = 0;
	float m_FPS = 0;

};