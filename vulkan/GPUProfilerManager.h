#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "GlobalSettings.h"
#include "mkList.h"

class VKCommandBuffer;

class GPUProfilerManager : public NonCopyable
{

	struct GPUTimeStampView
	{
		mkString name;
		float time;
		uint32_t depth;

		GPUTimeStampView(mkString n, float t, uint32_t d) :name(n), time(t), depth(d) {}
	};

	struct FrameGPUTimeStampView
	{
		uint32_t frameIndex;
		std::vector<GPUTimeStampView> gpuTimeStampViews;

		FrameGPUTimeStampView(uint32_t index) :frameIndex(index) {}

		mkString ToString();
	};

	struct QueryResource
	{
		uint32_t timeStampCount = 0;
		std::vector<mkString> names;
	};

public:

	GPUProfilerManager(VkDevice vkDevice);
	~GPUProfilerManager();

	void Reset(VKCommandBuffer* cb);
	void WriteTimeStamp(VKCommandBuffer* cb, mkString name);
	void ResolveTimeStamp();
	void Update();
	FrameGPUTimeStampView& GetLastFrameView();
	void WriteToFile();

private:

	const static uint32_t m_MaxQueryCount = 200;
	const uint32_t m_MaxFrameCount = 1000;

	uint32_t m_CurrFrameResourcesIndex = 0;
	QueryResource m_QueryResource[FrameResourcesCount];

	mkList<FrameGPUTimeStampView> m_FrameGPUTimeStampViews;

	uint32_t m_FrameIndex = 0;

	VkQueryPool m_QueryPool = VK_NULL_HANDLE;
	VkDevice m_Device = VK_NULL_HANDLE;
};