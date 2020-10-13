#include "VKQueryPool.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"

VKQueryPool::VKQueryPool(VKDevice * vkDevice, VkQueryType queryType, uint32_t queryCount) :
	device(vkDevice->device)
{
	VkQueryPoolCreateInfo ci;
	ci.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	ci.pNext = nullptr;
	ci.flags = 0;
	ci.pipelineStatistics = 0;
	ci.queryType = queryType;
	ci.queryCount = queryCount;

	vkCreateQueryPool(device, &ci, nullptr, &queryPool);
}

VKQueryPool::~VKQueryPool()
{
	if (device != VK_NULL_HANDLE && queryPool != VK_NULL_HANDLE) {
		vkDestroyQueryPool(device, queryPool, nullptr);
		queryPool = VK_NULL_HANDLE;
	}
}
