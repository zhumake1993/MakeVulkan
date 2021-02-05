#pragma once

#include "Env.h" 
#include "NonCopyable.h"

class TimeManager : public NonCopyable
{

public:

	TimeManager();
	~TimeManager();

	int GetFrameIndex();
	float GetDeltaTime();
	float GetFPS();
	void Update();

private:

	//

public:

	//

private:

	int m_FrameIndex = -1;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_LastTimestamp = std::chrono::high_resolution_clock::now();
	float m_DeltaTime = 0;

	int m_AccumulateCounter = 0;
	float m_AccumulateTime = 0;
	float m_FPS = 0;

};

//void CreateTimeManager();
//TimeManager& GetTimeManager();
//void ReleaseTimeManager();