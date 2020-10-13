#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;
struct VKQueryPool;
struct VKCommandBuffer;

class GPUProfilerMgr : public NonCopyable
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

	GPUProfilerMgr(VKDevice* vkDevice);
	~GPUProfilerMgr();

	void SetVKCommandBuffer(VKCommandBuffer* vkCommandBuffer);
	void Reset();
	void WriteTimeStamp(std::string name);
	void Tick();
	FrameGPUTimeStampView& GetLastFrameView();
	void WriteToFile();

private:
	
	//

private:

	const static uint32_t m_MaxQueryCount = 200;
	const uint32_t m_MaxFrameCount = 1000;

	uint32_t m_CurrFrameResourcesIndex = 0;
	QueryResource m_QueryResource[FrameResourcesCount];

	std::list<FrameGPUTimeStampView> m_FrameGPUTimeStampViews;

	VKQueryPool* m_QueryPool;
	VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
};