#include "UniformBufferMgr.h"
#include "VulkanDriver.h"
#include "Tools.h"
#include "ProfilerMgr.h"

UniformBufferMgr::UniformBufferMgr()
{
}

UniformBufferMgr::~UniformBufferMgr()
{
}

VKBuffer * UniformBufferMgr::GetUniformBuffer(std::string name, VkDeviceSize size)
{
	auto& driver = GetVulkanDriver();

	if (m_NameToUBCache.find(name) == m_NameToUBCache.end()) {
		m_NameToUBCache[name] = UBCache(size);
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
