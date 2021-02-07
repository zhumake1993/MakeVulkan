#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"

struct VKDescriptorSet : public VKResource
{
	VKDescriptorSet(VkDevice vkDevice, VkDescriptorPool vkDescriptorPool) :device(vkDevice), descriptorPool(vkDescriptorPool) {}
	virtual ~VKDescriptorSet()
	{
		vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
	}

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

	VkDescriptorSetLayout GetDSLGlobal();
	VkDescriptorSetLayout GetDSLPerView();

	VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);

private:

	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

	// Global和PerView的Layout预先设置，非自定义
	VkDescriptorSetLayout m_DSLGlobal = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_DSLPerView = VK_NULL_HANDLE;

	GarbageCollector* m_GarbageCollector = nullptr;

	VkDevice m_Device = VK_NULL_HANDLE;
};