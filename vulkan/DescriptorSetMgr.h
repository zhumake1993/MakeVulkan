#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "DescriptorSetTypes.h"

class DescriptorSetMgr : public NonCopyable
{
	using SetList = std::forward_list<VkDescriptorSet>;

	// 缓存ds
	struct SetCache {
		//SetList set0; // 空闲的ds的list
		//SetList set1;
		//SetList set2;
		//SetList set3;
		std::vector<SetList> sets; // sets[0]是空闲的ds的list

		SetCache() {
			sets.resize(FrameResourcesCount + 1);
		}
	};

public:

	DescriptorSetMgr(VkDevice device);
	~DescriptorSetMgr();

	VkDescriptorSetLayout CreateDescriptorSetLayout(DSLBindings& bindings);

	// persistent：用于gui，待修改
	VkDescriptorSet GetDescriptorSet(VkDescriptorSetLayout layout, bool persistent);
	void UpdateDescriptorSet(VkDescriptorSet set, DesUpdateInfos& infos);
	void Tick();

private:

	VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);
	VkDescriptorType GetBindingType(VkDescriptorSet set, uint32_t binding);

public:

	//

private:

	std::unordered_map<VkDescriptorSetLayout, std::vector<VkDescriptorType>> m_LayoutToBindingType;
	std::unordered_map<VkDescriptorSetLayout, SetCache> m_LayoutToSetCache;
	std::unordered_map<VkDescriptorSet, VkDescriptorSetLayout> m_SetToLayout;

	VkDevice m_Device = VK_NULL_HANDLE;
	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
};