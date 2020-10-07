#include "VKCommandBuffer.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"
#include "VKCommandPool.h"

VKCommandBuffer::VKCommandBuffer(VKDevice * vkDevice, VKCommandPool * vkCommandPool, VkCommandBufferLevel level) :
	device(vkDevice->device),
	commandPool(vkCommandPool->commandPool)
{
}

VKCommandBuffer::~VKCommandBuffer()
{
}
