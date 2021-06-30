#include "ProfilerManager.h"
#include "Tools.h"
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>

ProfilerManager* gProfilerManager;

void CreateProfilerManager()
{
	gProfilerManager = new ProfilerManager();
}

ProfilerManager & GetProfilerManager()
{
	return *gProfilerManager;
}

void ReleaseProfilerManager()
{
	RELEASE(gProfilerManager);
}

ProfilerManager::ProfilerManager()
{

}

ProfilerManager::~ProfilerManager()
{

}

void ProfilerManager::Update()
{
	m_FrameIndex++;
}

void ProfilerManager::RecordTime(mkString name)
{
	if (m_FrameStamps.empty() || m_FrameStamps.back().frameIndex != m_FrameIndex) {
		m_FrameStamps.emplace_back(m_FrameIndex);
	}

	FrameStamp& frameStamp = m_FrameStamps.back();
	frameStamp.stamps.emplace_back(name, std::chrono::high_resolution_clock::now());

	if (m_FrameStamps.size() > m_MaxFrameCount) {
		m_FrameStamps.pop_front();
	}
}

ProfilerManager::FrameTimeView ProfilerManager::Resolve(uint32_t frameIndex)
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

void ProfilerManager::WriteToFile()
{
#if defined(_WIN32)
	mkString filePath = "CPUProfiler.txt";
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	mkString filePath = "/data/data/com.example.MakeVulkan/CPUProfiler.txt";
#endif

	std::ofstream outfile;
	outfile.open(filePath);

	for (auto ir = m_FrameStamps.begin(); ir != m_FrameStamps.end(); ir++) {
		outfile << Resolve(*ir).ToString() << std::endl;
	}

	outfile.close();
}

ProfilerManager::FrameTimeView ProfilerManager::Resolve(FrameStamp& frameStamp)
{
	mkVector<Stamp>& stamps = frameStamp.stamps;

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
			float time = (stampStack.top().time - stamps[i].time).count() / 1000000.0f;
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

ProfilerScope::ProfilerScope(mkString name):
	m_Name(name)
{
	gProfilerManager->RecordTime(m_Name);
}

ProfilerScope::~ProfilerScope()
{
	gProfilerManager->RecordTime(m_Name);
}

mkString ProfilerManager::FrameTimeView::ToString()
{
	std::ostringstream ostr;

	ostr << "FrameIndex: " << frameIndex << std::endl;
	for (auto& timeView : timeViews) {
		ostr << mkString(timeView.depth, '\t') << timeView.name << "   " << timeView.time << std::endl;
	}

	return ostr.str();
}
