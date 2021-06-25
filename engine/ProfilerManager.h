#pragma once

#include "NonCopyable.h"
#include <chrono>

class ProfilerManager : public NonCopyable
{
	// 记录的原始数据结构
	struct Stamp
	{
		std::string name;
		std::chrono::time_point<std::chrono::high_resolution_clock> time;

		Stamp(std::string s, std::chrono::time_point<std::chrono::high_resolution_clock> t) :name(s), time(t) {}
	};

	struct FrameStamp
	{
		uint32_t frameIndex;
		std::vector<Stamp> stamps;

		FrameStamp() :frameIndex(-1) {}
		FrameStamp(uint32_t index) :frameIndex(index) {}
	};

	// 处理后的方便显示的数据结构
	struct TimeView
	{
		std::string name;
		float time;
		uint32_t depth;

		TimeView(std::string n, float t, uint32_t d) :name(n), time(t), depth(d) {}
	};

	struct FrameTimeView
	{
		uint32_t frameIndex;
		std::vector<TimeView> timeViews;

		FrameTimeView(uint32_t index) :frameIndex(index) {}

		std::string ToString();
	};

public:

	ProfilerManager();
	~ProfilerManager();

	void Update();
	void RecordTime(std::string name);
	FrameTimeView Resolve(uint32_t frameIndex);
	void WriteToFile();

private:

	FrameTimeView Resolve(FrameStamp& frameStamp);

private:

	std::list<FrameStamp> m_FrameStamps;
	uint32_t m_MaxFrameCount = 1000;

	int m_FrameIndex = -1;
};

class ProfilerScope
{

public:

	ProfilerScope(std::string name);
	~ProfilerScope();

private:

	std::string m_Name;
};

void CreateProfilerManager();
ProfilerManager& GetProfilerManager();
void ReleaseProfilerManager();

#define PROFILER(name) ProfilerScope name##ProfilerScope(#name)
#define PROFILER_BEGIN(name) GetProfilerManager().RecordTime(#name)
#define PROFILER_END(name) GetProfilerManager().RecordTime(#name)