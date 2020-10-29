#include "UniformBufferMgr.h"
#include "VulkanDriver.h"
#include "VKBuffer.h"
#include "Tools.h"
#include "ProfilerMgr.h"

UniformBufferMgr::UniformBufferMgr()
{
}

UniformBufferMgr::~UniformBufferMgr()
{
}

void UniformBufferMgr::CreateUniformBuffer(std::string name, VkDeviceSize size)
{
	if (m_NameToUBCache.find(name) != m_NameToUBCache.end()) {
		LOG("uniform buffer already exists!");
		assert(false);
	}

	m_NameToUBCache.emplace(name, size);
	//m_NameToUBCache[name] = UBCache(size);
}

VKBuffer * UniformBufferMgr::GetUniformBuffer(std::string name)
{
	if (m_NameToUBCache.find(name) == m_NameToUBCache.end()) {
		LOG("uniform buffer does not exist!");
		assert(false);
	}
	
	return m_NameToUBCache[name].GetBuffer();
}

void UniformBufferMgr::Tick()
{
	PROFILER(UniformBufferMgr_Tick);

	for (auto& pair : m_NameToUBCache) {
		pair.second.Tick();
	}
}

UniformBufferMgr::UBCache::UBCache()
{
}

UniformBufferMgr::UBCache::UBCache(VkDeviceSize size)
{
	buffers.resize(FrameResourcesCount);

	for (int i = 0; i < FrameResourcesCount; i++) {
		buffers[i] = GetVulkanDriver().CreateVKBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		buffers[i]->Map();
	}
}

UniformBufferMgr::UBCache::~UBCache()
{
	for (int i = 0; i < FrameResourcesCount; i++) {
		RELEASE(buffers[i]);
	}
}

VKBuffer * UniformBufferMgr::UBCache::GetBuffer()
{
	return buffers[index];
}

void UniformBufferMgr::UBCache::Tick()
{
	index = (index + 1) % FrameResourcesCount;
}