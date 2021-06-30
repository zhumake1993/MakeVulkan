#include "GPUProfilerManager.h"
#include "VKTools.h"
#include "VKCommandBuffer.h"
#include "DeviceProperties.h"
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>

GPUProfilerManager::GPUProfilerManager(VkDevice vkDevice) :
	m_Device(vkDevice)
{
	VkQueryPoolCreateInfo ci;
	ci.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	ci.pNext = nullptr;
	ci.flags = 0;
	ci.pipelineStatistics = 0;
	ci.queryType = VK_QUERY_TYPE_TIMESTAMP;
	ci.queryCount = m_MaxQueryCount * FrameResourcesCount;

	vkCreateQueryPool(m_Device, &ci, nullptr, &m_QueryPool);

	m_FrameGPUTimeStampViews.emplace_back(0);
}

GPUProfilerManager::~GPUProfilerManager()
{
	vkDestroyQueryPool(m_Device, m_QueryPool, nullptr);
}

void GPUProfilerManager::Reset(VKCommandBuffer* cb)
{
	QueryResource& queryResource = m_QueryResource[m_CurrFrameResourcesIndex];

	queryResource.timeStampCount = 0;
	queryResource.names.clear();

	vkCmdResetQueryPool(cb->commandBuffer, m_QueryPool, m_CurrFrameResourcesIndex * m_MaxQueryCount, m_MaxQueryCount);
}

void GPUProfilerManager::WriteTimeStamp(VKCommandBuffer* cb, mkString name)
{
	QueryResource& queryResource = m_QueryResource[m_CurrFrameResourcesIndex];

	if (queryResource.timeStampCount >= m_MaxQueryCount) {
		LOG("too many time stamps!");
		assert(false);
	}

	queryResource.names.push_back(name);
	vkCmdWriteTimestamp(cb->commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_QueryPool, m_CurrFrameResourcesIndex * m_MaxQueryCount + queryResource.timeStampCount);
	queryResource.timeStampCount++;
}

void GPUProfilerManager::ResolveTimeStamp()
{
	if (m_FrameIndex < FrameResourcesCount)
	{
		return;
	}

	// 此时m_FrameIndex - FrameResourcesCount帧已经执行完gpu，可以获取gpu timestamp

	auto& dp = GetDeviceProperties();

	QueryResource& queryResource = m_QueryResource[m_CurrFrameResourcesIndex];

	mkVector<uint64_t> timeStamps(queryResource.timeStampCount);
	mkVector<float> timeResults(queryResource.timeStampCount);

	// parameter dataSize must be greater than 0
	if (queryResource.timeStampCount > 0)
	{
		// 使用VK_QUERY_RESULT_WAIT_BIT的情况下，如果查询的query没有写入timeStamp的话，会报错
		VK_CHECK_RESULT(vkGetQueryPoolResults(m_Device, m_QueryPool, 0, queryResource.timeStampCount,
			sizeof(uint64_t) * queryResource.timeStampCount, timeStamps.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT));
	}

	for (uint32_t i = 0; i < queryResource.timeStampCount; i++)
	{
		timeResults[i] = timeStamps[i] * dp.deviceProperties.limits.timestampPeriod / 1000000.0f;
	}

	// resolve

	m_FrameGPUTimeStampViews.emplace_back(m_FrameIndex - FrameResourcesCount);

	FrameGPUTimeStampView& view = m_FrameGPUTimeStampViews.back();

	std::stack<std::pair<mkString, float>> stampStack;
	int depth = 0;

	for (int i = static_cast<int>(queryResource.names.size()) - 1; i >= 0; i--)
	{
		if (stampStack.empty() || stampStack.top().first != queryResource.names[i])
		{
			depth++;
			stampStack.emplace(queryResource.names[i], timeResults[i]);
		}
		else
		{
			depth--;
			float time = stampStack.top().second - timeResults[i];
			view.gpuTimeStampViews.emplace_back(queryResource.names[i], time, depth);
			stampStack.pop();
		}
	}

	if (!stampStack.empty())
	{
		LOG("timestamp scope not match!");
		assert(false);
	}

	std::reverse(view.gpuTimeStampViews.begin(), view.gpuTimeStampViews.end());

	if (m_FrameGPUTimeStampViews.size() > m_MaxFrameCount)
	{
		m_FrameGPUTimeStampViews.pop_front();
	}
}

void GPUProfilerManager::Update()
{
	m_FrameIndex++;
	m_CurrFrameResourcesIndex = (m_CurrFrameResourcesIndex + 1) % FrameResourcesCount;
}

GPUProfilerManager::FrameGPUTimeStampView & GPUProfilerManager::GetLastFrameView()
{
	return m_FrameGPUTimeStampViews.back();
}

void GPUProfilerManager::WriteToFile()
{
#if defined(_WIN32)
	mkString filePath = "GPUProfiler.txt";
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	mkString filePath = "/data/data/com.example.MakeVulkan/GPUProfiler.txt";
#endif

	std::ofstream outfile;
	outfile.open(filePath);

	for (auto ir = m_FrameGPUTimeStampViews.begin(); ir != m_FrameGPUTimeStampViews.end(); ir++) {
		outfile << ir->ToString() << std::endl;
	}

	outfile.close();
}

mkString GPUProfilerManager::FrameGPUTimeStampView::ToString()
{
	std::ostringstream ostr;

	ostr << "FrameIndex: " << frameIndex << std::endl;
	for (auto& view : gpuTimeStampViews) {
		ostr << mkString(view.depth, '\t') << view.name << "   " << view.time << std::endl;
	}

	return ostr.str();
}
