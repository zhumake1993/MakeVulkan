#include "VKCommandPool.h"
#include "VKTools.h"

VKCommandPool::VKCommandPool(VkDevice vkDevice, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex) :
	device(vkDevice)
{
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = nullptr;
	cmdPoolInfo.flags = flags;
	cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;

	VK_CHECK_RESULT(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &commandPool));
}

VKCommandPool::~VKCommandPool()
{
	if (device != VK_NULL_HANDLE && commandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(device, commandPool, nullptr);
		commandPool = VK_NULL_HANDLE;
	}
}
