#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "DescriptorSetTypes.h"

class DescriptorSetMgr : public NonCopyable
{
	using SetList = std::forward_list<VkDescriptorSet>;

	struct SetCache {
		SetList set0;
		SetList set1;
		SetList set2;
		SetList set3;
	};

public:

	DescriptorSetMgr(VkDevice device);
	~DescriptorSetMgr();

	VkDescriptorSetLayout CreateDescriptorSetLayout(DSLBindings& bindings);
	VkDescriptorSet GetDescriptorSet(VkDescriptorSetLayout layout);
	void UpdateDescriptorSet(VkDescriptorSet set, DesUpdateInfos& infos);
	void Tick();

private:

	VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);
	VkDescriptorType GetBindingType(VkDescriptorSet set, uint32_t binding);

public:

	//

private:

	VkDevice m_Device = VK_NULL_HANDLE;
	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
	std::unordered_map<VkDescriptorSetLayout, std::vector<VkDescriptorType>> m_LayoutBindingTypes;
	std::unordered_map<VkDescriptorSetLayout, SetCache> m_SetCaches;
	std::unordered_map<VkDescriptorSet, VkDescriptorSetLayout> m_SetToLayout;
};