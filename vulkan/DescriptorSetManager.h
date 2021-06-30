#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include "VKResource.h"
#include "mkList.h"
#include "mkHashMap.h"

class DescriptorSetManager : public NonCopyable
{

	struct DescriptorSet : public vk::VKResource
	{
		DescriptorSet() {}
		~DescriptorSet() {}

		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	};

public:

	DescriptorSetManager(VkDevice vkDevice);
	virtual ~DescriptorSetManager();

	void Update();

	VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);

private:

	DescriptorSet* AllocateDescriptorSetInternal(VkDescriptorSetLayout layout);

private:

	using SetList = mkList<DescriptorSet*>;
	mkHashMap<VkDescriptorSetLayout, SetList> m_SetCache;

	uint32_t m_FrameIndex = 0;

	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

	VkDevice m_Device = VK_NULL_HANDLE;
};