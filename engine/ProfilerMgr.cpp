#include "ProfilerMgr.h"
#include "TimeMgr.h"
#include "Tools.h"
#include <stack>
#include <algorithm>
#include <sstream>

ProfilerMgr* gProfilerMgr;

void CreateProfilerMgr()
{
	gProfilerMgr = new ProfilerMgr();
}

ProfilerMgr & GetProfilerMgr()
{
	return *gProfilerMgr;
}

void ReleaseProfilerMgr()
{
	RELEASE(gProfilerMgr);
}

ProfilerMgr::ProfilerMgr()
{

}

ProfilerMgr::~ProfilerMgr()
{

}

void ProfilerMgr::RecordTime(std::string name)
{
	uint32_t frameIndex = GetTimeMgr().GetFrameIndex();

	if (m_FrameStamps.empty() || m_FrameStamps.back().frameIndex != frameIndex) {
		m_FrameStamps.emplace_back(frameIndex);
	}

	FrameStamp& frameStamp = m_FrameStamps.back();
	frameStamp.stamps.emplace_back(name, std::chrono::high_resolution_clock::now());

	if (m_FrameStamps.size() > m_MaxFrameCount) {
		m_FrameStamps.pop_front();
	}
}

ProfilerMgr::FrameTimeView ProfilerMgr::Resolve(uint32_t frameIndex)
{
	if (m_FrameStamps.empty()) {
		return FrameTimeView(frameIndex);
	}

	if (frameIndex < m_FrameStamps.front().frameIndex || frameIndex>m_FrameStamps.back().frameIndex) {
		return FrameTimeView(frameIndex);
	}

	for (auto ir = m_FrameStamps.rbegin(); ir != m_FrameStamps.rend(); ir++) {
		if (ir->frameIndex == frameIndex) {
			return Resolve(*ir);
		}
	}

	LOG("wrong frameIndex");
	assert(false);

	return FrameTimeView(frameIndex);
}

ProfilerMgr::FrameTimeView ProfilerMgr::Resolve(FrameStamp& frameStamp)
{
	std::vector<Stamp>& stamps = frameStamp.stamps;

	FrameTimeView frameTimeView(frameStamp.frameIndex);
	std::stack<Stamp> stampStack;
	int depth = 0;

	for (int i = static_cast<int>(stamps.size()) - 1; i >= 0; i--) {
		if (stampStack.empty() || stampStack.top().name != stamps[i].name) {
			depth++;
			stampStack.push(stamps[i]);
		}
		else {
			depth--;
			float time = (stampStack.top().time - stamps[i].time).count() / 1000000000.0f;
			frameTimeView.timeViews.emplace_back(stamps[i].name, time, depth);
			stampStack.pop();
		}
	}

	if (!stampStack.empty()) {
		LOG("profiler scope not match!");
		assert(false);
	}

	std::reverse(frameTimeView.timeViews.begin(), frameTimeView.timeViews.end());

	return frameTimeView;
}

ProfilerScope::ProfilerScope(std::string name):
	m_Name(name)
{
	gProfilerMgr->RecordTime(m_Name);
}

ProfilerScope::~ProfilerScope()
{
	gProfilerMgr->RecordTime(m_Name);
}

std::string ProfilerMgr::FrameTimeView::ToString()
{
	std::ostringstream ostr;

	ostr << "FrameIndex: " << frameIndex << std::endl;
	for (auto& timeView : timeViews) {
		ostr << std::string(timeView.depth, '\t') << timeView.name << "   " << timeView.time << std::endl;
	}

	return ostr.str();
}