#include "VKFence.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"

VKFence::VKFence(VKDevice * vkDevice, bool signaled) :
	device(vkDevice->device)
{
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));
}

VKFence::~VKFence()
{
	if (device != VK_NULL_HANDLE && fence != VK_NULL_HANDLE) {
		vkDestroyFence(device, fence, nullptr);
		fence = VK_NULL_HANDLE;
	}
}

void VKFence::Wait()
{
	VK_CHECK_RESULT(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));
}

void VKFence::Reset()
{
	VK_CHECK_RESULT(vkResetFences(device, 1, &fence));
}