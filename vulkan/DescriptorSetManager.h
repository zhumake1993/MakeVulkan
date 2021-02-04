#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "VKResource.h"

union DescriptorInfo
{
	VkDescriptorBufferInfo buffer;
	VkDescriptorImageInfo image;
	VkBufferView texelBufferView;
};

struct VKDescriptorSet : public VKResource
{
	VKDescriptorSet(uint32_t currFrameIndex) :VKResource(currFrameIndex) {}
	virtual ~VKDescriptorSet() {}

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

class DescriptorSetManager : public NonCopyable
{
public:

	DescriptorSetManager(VkDevice vkDevice);
	virtual ~DescriptorSetManager();

	void Update();

	VkDescriptorSetLayout GetDSLGlobal();
	VkDescriptorSetLayout GetDSLPerView();

	VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);

private:

	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

	// Global和PerView的Layout预先设置，非自定义
	VkDescriptorSetLayout m_DSLGlobal = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_DSLPerView = VK_NULL_HANDLE;

	// 这一帧新加的DescriptorSet
	std::list<VKDescriptorSet*> m_NewDescriptorSets;

	// 可能还在使用中的DescriptorSet
	std::list<VKDescriptorSet*> m_PendingDescriptorSets;

	uint32_t m_FrameIndex = 0;

	VkDevice m_Device = VK_NULL_HANDLE;
};