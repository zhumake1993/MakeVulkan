#pragma once

#include "Env.h"
#include "NonCopyable.h"

class VKResource;

class GarbageCollector : public NonCopyable
{
public:

	GarbageCollector();
	virtual ~GarbageCollector();

	void Update();

	uint32_t GetFrameIndex();

	void AddResource(VKResource* resource);

private:

	// ��һ֡�¼ӵ�Resource
	std::list<VKResource*> m_NewResources;

	// ���ܻ���ʹ���е�Resource
	std::list<VKResource*> m_PendingResources;

	uint32_t m_FrameIndex = 0;

	VkDevice m_Device = VK_NULL_HANDLE;
	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
};