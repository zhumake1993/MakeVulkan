#pragma once

#include "Common.h"
#include "NonCopyable.h"

class ProfilerMgr : public NonCopyable
{
	// ��¼��ԭʼ���ݽṹ
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

	// �����ķ�����ʾ�����ݽṹ
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

	ProfilerMgr();
	~ProfilerMgr();

	void RecordTime(std::string name);
	FrameTimeView Resolve(uint32_t frameIndex);

private:

	FrameTimeView Resolve(FrameStamp& frameStamp);

private:

	std::list<FrameStamp> m_FrameStamps;
	uint32_t m_MaxFrameCount = 1000;
};

class ProfilerScope
{

public:

	ProfilerScope(std::string name);
	~ProfilerScope();

private:

	std::string m_Name;
};

void CreateProfilerMgr();
ProfilerMgr& GetProfilerMgr();
void ReleaseProfilerMgr();

#define PROFILER(name) ProfilerScope name##ProfilerScope(#name)