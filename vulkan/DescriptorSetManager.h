#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"

struct VKDescriptorSet : public VKResource
{
	VKDescriptorSet(VkDevice vkDevice, VkDescriptorPool vkDescriptorPool) :device(vkDevice), descriptorPool(vkDescriptorPool) {}
	virtual ~VKDescriptorSet();

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
};

class GarbageCollector;

class DescriptorSetManager : public NonCopyable
{
public:

	DescriptorSetManager(VkDevice vkDevice, GarbageCollector* gc);
	virtual ~DescriptorSetManager();

	VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);

private:

	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

	GarbageCollector* m_GarbageCollector = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;
};