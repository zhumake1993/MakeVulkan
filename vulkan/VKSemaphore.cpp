#include "VKSemaphore.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"

VKSemaphore::VKSemaphore(VKDevice * vkDevice) :
	device(vkDevice->device)
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore));
}

VKSemaphore::~VKSemaphore()
{
	if (device != VK_NULL_HANDLE && semaphore != VK_NULL_HANDLE) {
		vkDestroySemaphore(device, semaphore, nullptr);
		semaphore = VK_NULL_HANDLE;
	}
}
