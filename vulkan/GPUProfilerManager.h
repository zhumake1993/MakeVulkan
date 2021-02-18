#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "Settings.h"

struct VKCommandBuffer;

class GPUProfilerManager : public NonCopyable
{

	struct GPUTimeStampView
	{
		std::string name;
		float time;
		uint32_t depth;

		GPUTimeStampView(std::string n, float t, uint32_t d) :name(n), time(t), depth(d) {}
	};

	struct FrameGPUTimeStampView
	{
		uint32_t frameIndex;
		std::vector<GPUTimeStampView> gpuTimeStampViews;

		FrameGPUTimeStampView(uint32_t index) :frameIndex(index) {}

		std::string ToString();
	};

	struct QueryResource
	{
		uint32_t timeStampCount = 0;
		std::vector<std::string> names;
	};

public:

	GPUProfilerManager(VkDevice vkDevice);
	~GPUProfilerManager();

	void Reset(VKCommandBuffer* cb);
	void WriteTimeStamp(VKCommandBuffer* cb, std::string name);
	void ResolveTimeStamp();
	void Update();
	FrameGPUTimeStampView& GetLastFrameView();
	void WriteToFile();

private:

	const static uint32_t m_MaxQueryCount = 200;
	const uint32_t m_MaxFrameCount = 1000;

	uint32_t m_CurrFrameResourcesIndex = 0;
	QueryResource m_QueryResource[FrameResourcesCount];

	std::list<FrameGPUTimeStampView> m_FrameGPUTimeStampViews;

	uint32_t m_FrameIndex = 0;

	VkQueryPool m_QueryPool = VK_NULL_HANDLE;
	VkDevice m_Device = VK_NULL_HANDLE;
};