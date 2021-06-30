#pragma once

#include "NonCopyable.h"
#include <chrono>
#include "mkList.h"
#include "mkVector.h"
#include "mkString.h"

class ProfilerManager : public NonCopyable
{
	// 记录的原始数据结构
	struct Stamp
	{
		mkString name;
		std::chrono::time_point<std::chrono::high_resolution_clock> time;

		Stamp(mkString s, std::chrono::time_point<std::chrono::high_resolution_clock> t) :name(s), time(t) {}
	};

	struct FrameStamp
	{
		uint32_t frameIndex;
		mkVector<Stamp> stamps;

		FrameStamp() :frameIndex(-1) {}
		FrameStamp(uint32_t index) :frameIndex(index) {}
	};

	// 处理后的方便显示的数据结构
	struct TimeView
	{
		mkString name;
		float time;
		uint32_t depth;

		TimeView(mkString n, float t, uint32_t d) :name(n), time(t), depth(d) {}
	};

	struct FrameTimeView
	{
		uint32_t frameIndex;
		mkVector<TimeView> timeViews;

		FrameTimeView(uint32_t index) :frameIndex(index) {}

		mkString ToString();
	};

public:

	ProfilerManager();
	~ProfilerManager();

	void Update();
	void RecordTime(mkString name);
	FrameTimeView Resolve(uint32_t frameIndex);
	void WriteToFile();

private:

	FrameTimeView Resolve(FrameStamp& frameStamp);

private:

	mkList<FrameStamp> m_FrameStamps;
	uint32_t m_MaxFrameCount = 1000;

	int m_FrameIndex = -1;
};

class ProfilerScope
{

public:

	ProfilerScope(mkString name);
	~ProfilerScope();

private:

	mkString m_Name;
};

void CreateProfilerManager();
ProfilerManager& GetProfilerManager();
void ReleaseProfilerManager();

#define PROFILER(name) ProfilerScope name##ProfilerScope(#name)
#define PROFILER_BEGIN(name) GetProfilerManager().RecordTime(#name)
#define PROFILER_END(name) GetProfilerManager().RecordTime(#name)