#include "GPUProfilerMgr.h"
#include "Tools.h"
#include "TimeMgr.h"
#include "DeviceProperties.h"
#include "VulkanDriver.h"
#include "VKDevice.h"
#include "VKQueryPool.h"
#include "VKCommandBuffer.h"
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>

GPUProfilerMgr::GPUProfilerMgr(VKDevice* vkDevice) :
	device(vkDevice->device)
{
	auto& driver = GetVulkanDriver();

	m_QueryPool = driver.CreateVKQueryPool(VK_QUERY_TYPE_TIMESTAMP, m_MaxQueryCount * FrameResourcesCount);

	m_FrameGPUTimeStampViews.emplace_back(0);
}

GPUProfilerMgr::~GPUProfilerMgr()
{
	RELEASE(m_QueryPool);
	m_CommandBuffer = VK_NULL_HANDLE;
}

void GPUProfilerMgr::SetVKCommandBuffer(VKCommandBuffer * vkCommandBuffer)
{
	m_CommandBuffer = vkCommandBuffer->commandBuffer;
}

void GPUProfilerMgr::Reset()
{
	QueryResource& queryResource = m_QueryResource[m_CurrFrameResourcesIndex];

	queryResource.timeStampCount = 0;
	queryResource.names.clear();

	vkCmdResetQueryPool(m_CommandBuffer, m_QueryPool->queryPool, m_CurrFrameResourcesIndex * m_MaxQueryCount, m_MaxQueryCount);
}

void GPUProfilerMgr::WriteTimeStamp(std::string name)
{
	QueryResource& queryResource = m_QueryResource[m_CurrFrameResourcesIndex];

	if (queryResource.timeStampCount >= m_MaxQueryCount) {
		LOG("too many time stamps!");
		assert(false);
	}

	queryResource.names.push_back(name);
	vkCmdWriteTimestamp(m_CommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_QueryPool->queryPool, m_CurrFrameResourcesIndex * m_MaxQueryCount + queryResource.timeStampCount);
	queryResource.timeStampCount++;
}

void GPUProfilerMgr::Tick()
{
	m_CurrFrameResourcesIndex = (m_CurrFrameResourcesIndex + 1) % FrameResourcesCount;

	uint32_t frameIndex = GetTimeMgr().GetFrameIndex();
	if (frameIndex <= FrameResourcesCount) {
		return;
	}

	// 此时frameIndex - FrameResourcesCount帧已经执行完gpu，可以获取gpu timestamp

	auto& dp = GetDeviceProperties();

	QueryResource& queryResource = m_QueryResource[m_CurrFrameResourcesIndex];

	std::vector<uint64_t> timeStamps(queryResource.timeStampCount);
	std::vector<float> timeResults(queryResource.timeStampCount);

	// parameter dataSize must be greater than 0
	if (queryResource.timeStampCount > 0) {
		// 使用VK_QUERY_RESULT_WAIT_BIT的情况下，如果查询的query没有写入timeStamp的话，会报错
		VK_CHECK_RESULT(vkGetQueryPoolResults(device, m_QueryPool->queryPool, 0, queryResource.timeStampCount,
			sizeof(uint64_t) * queryResource.timeStampCount, timeStamps.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT));
	}

	for (uint32_t i = 0; i < queryResource.timeStampCount; i++) {
		timeResults[i] = timeStamps[i] * dp.deviceProperties.limits.timestampPeriod / 1000000.0f;
	}

	// resolve

	m_FrameGPUTimeStampViews.emplace_back(frameIndex - FrameResourcesCount);

	FrameGPUTimeStampView& view = m_FrameGPUTimeStampViews.back();
	
	std::stack<std::pair<std::string, float>> stampStack;
	int depth = 0;

	for (int i = static_cast<int>(queryResource.names.size()) - 1; i >= 0; i--) {
		if (stampStack.empty() || stampStack.top().first != queryResource.names[i]) {
			depth++;
			stampStack.emplace(queryResource.names[i], timeResults[i]);
		}
		else {
			depth--;
			float time = stampStack.top().second - timeResults[i];
			view.gpuTimeStampViews.emplace_back(queryResource.names[i], time, depth);
			stampStack.pop();
		}
	}

	if (!stampStack.empty()) {
		LOG("timestamp scope not match!");
		assert(false);
	}

	std::reverse(view.gpuTimeStampViews.begin(), view.gpuTimeStampViews.end());

	if (m_FrameGPUTimeStampViews.size() > m_MaxFrameCount) {
		m_FrameGPUTimeStampViews.pop_front();
	}
}

GPUProfilerMgr::FrameGPUTimeStampView & GPUProfilerMgr::GetLastFrameView()
{
	return m_FrameGPUTimeStampViews.back();
}

void GPUProfilerMgr::WriteToFile()
{
#if defined(_WIN32)
	std::string filePath = "GPUProfiler.txt";
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	std::string filePath = "/data/data/com.example.MakeVulkan/GPUProfiler.txt";
#endif

	std::ofstream outfile;
	outfile.open(filePath);

	for (auto ir = m_FrameGPUTimeStampViews.begin(); ir != m_FrameGPUTimeStampViews.end(); ir++) {
		outfile << ir->ToString() << std::endl;
	}

	outfile.close();
}

std::string GPUProfilerMgr::FrameGPUTimeStampView::ToString()
{
	std::ostringstream ostr;

	ostr << "FrameIndex: " << frameIndex << std::endl;
	for (auto& view : gpuTimeStampViews) {
		ostr << std::string(view.depth, '\t') << view.name << "   " << view.time << std::endl;
	}

	return ostr.str();
}
