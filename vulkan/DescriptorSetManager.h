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

	// Global��PerView��LayoutԤ�����ã����Զ���
	VkDescriptorSetLayout m_DSLGlobal = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_DSLPerView = VK_NULL_HANDLE;

	// ��һ֡�¼ӵ�DescriptorSet
	std::list<VKDescriptorSet*> m_NewDescriptorSets;

	// ���ܻ���ʹ���е�DescriptorSet
	std::list<VKDescriptorSet*> m_PendingDescriptorSets;

	uint32_t m_FrameIndex = 0;

	VkDevice m_Device = VK_NULL_HANDLE;
};