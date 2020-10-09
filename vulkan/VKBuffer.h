#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;

struct VKBuffer : public NonCopyable
{
	VKBuffer(VKDevice* vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	~VKBuffer();

	void Map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Unmap();
	void Copy(void * data, VkDeviceSize offset, VkDeviceSize size);
	void Flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
	void Invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

	VkBuffer buffer = VK_NULL_HANDLE;

private:

	VkDeviceMemory memory = VK_NULL_HANDLE;
	void* mappedPointer = nullptr;

	VkDevice device = VK_NULL_HANDLE;
};