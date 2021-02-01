#pragma once

#include "Env.h"
#include "NonCopyable.h"

class VKBuffer;
class VKDescriptorSet;

class VKGarbageCollector : public NonCopyable
{
public:

	VKGarbageCollector(VkDevice vkDevice, VkDescriptorPool pool);
	~VKGarbageCollector();

	void Update(uint32_t currFrameIndex);

	void AddBuffer(VKBuffer* buffer);
	void AddDescriptorSet(VKDescriptorSet* descriptorSet);

private:

	std::list<VKBuffer*> m_PendingBuffers;
	std::list<VKDescriptorSet*> m_PendingDescriptorSets;

	VkDevice m_Device = VK_NULL_HANDLE;
	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
};