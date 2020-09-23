#include "DescriptorSetMgr.h"
#include "VulkanDriver.h"
#include "Tools.h"

DescriptorSetMgr::DescriptorSetMgr(VkDevice device):
	m_Device(device)
{
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes(11);
	descriptorPoolSizes[0] = { VK_DESCRIPTOR_TYPE_SAMPLER , 100 };
	descriptorPoolSizes[1] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER , 100 };
	descriptorPoolSizes[2] = { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE , 100 };
	descriptorPoolSizes[3] = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE , 100 };
	descriptorPoolSizes[4] = { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER , 100 };
	descriptorPoolSizes[5] = { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER , 100 };
	descriptorPoolSizes[6] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER , 100 };
	descriptorPoolSizes[7] = { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER , 100 };
	descriptorPoolSizes[8] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC , 100 };
	descriptorPoolSizes[9] = { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC , 100 };
	descriptorPoolSizes[10] = { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT , 100 };

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = nullptr;
	descriptorPoolCreateInfo.flags = 0;
	descriptorPoolCreateInfo.maxSets = 100;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

	VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));
}

DescriptorSetMgr::~DescriptorSetMgr()
{
	for (auto& pair : m_SetCaches) {
		vkDestroyDescriptorSetLayout(m_Device, pair.first, nullptr);
	}

	// 销毁DescriptorPool会自动销毁其中分配的Set
	// vkFreeDescriptorSets(m_VulkanDevice->m_LogicalDevice, m_VulkanDescriptorPool->m_DescriptorPool, 1, &m_DescriptorSet);

	vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
}

VkDescriptorSetLayout DescriptorSetMgr::CreateDescriptorSetLayout(DSLBindings & bindings)
{
	VkDescriptorSetLayout descriptorSetLayout;

	uint32_t num = static_cast<uint32_t>(bindings.size());

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings(num);
	std::vector<VkDescriptorType> bindingTypes(num);

	for (uint32_t i = 0; i < num; i++) {
		layoutBindings[i].binding = i;
		layoutBindings[i].descriptorType = bindings[i].descriptorType;
		layoutBindings[i].descriptorCount = bindings[i].descriptorCount;
		layoutBindings[i].stageFlags = bindings[i].stageFlags;
		layoutBindings[i].pImmutableSamplers = nullptr;

		bindingTypes[i] = bindings[i].descriptorType;
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

	m_LayoutBindingTypes[descriptorSetLayout] = bindingTypes;

	return descriptorSetLayout;
}

VkDescriptorSet DescriptorSetMgr::GetDescriptorSet(VkDescriptorSetLayout layout, bool persistent)
{
	if (persistent) {
		auto set = AllocateDescriptorSet(layout);

		return set;
	}
	else {
		if (m_SetCaches.find(layout) == m_SetCaches.end()) {
			m_SetCaches[layout] = SetCache();

			auto set = AllocateDescriptorSet(layout);
			m_SetCaches[layout].set3.push_front(set);

			return set;
		}
		else {
			auto& cache = m_SetCaches[layout];

			if (cache.set0.empty()) {
				auto set = AllocateDescriptorSet(layout);
				cache.set3.push_front(set);

				return set;
			}
			else {
				auto set = cache.set0.front();
				cache.set0.pop_front();
				cache.set3.push_front(set);

				return set;
			}
		}
	}
}

void DescriptorSetMgr::UpdateDescriptorSet(VkDescriptorSet set, DesUpdateInfos & infos)
{
	uint32_t num = static_cast<uint32_t>(infos.size());

	std::vector<VkWriteDescriptorSet> writeDescriptorSets(num);
	for (uint32_t i = 0; i < num; i++) {
		writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[i].pNext = nullptr;
		writeDescriptorSets[i].dstSet = set;
		writeDescriptorSets[i].dstBinding = infos[i].binding;
		writeDescriptorSets[i].dstArrayElement = 0;
		writeDescriptorSets[i].descriptorCount = 1;
		writeDescriptorSets[i].descriptorType = GetBindingType(set, infos[i].binding);
		writeDescriptorSets[i].pImageInfo = &infos[i].info.image;
		writeDescriptorSets[i].pBufferInfo = &infos[i].info.buffer;
		writeDescriptorSets[i].pTexelBufferView = &infos[i].info.texelBufferView;
	}

	vkUpdateDescriptorSets(m_Device, num, writeDescriptorSets.data(), 0, nullptr);
}

void DescriptorSetMgr::Tick()
{
	for (auto& pair : m_SetCaches) {
		auto& cache = pair.second;

		cache.set0.splice_after(cache.set0.before_begin(), cache.set1);
		cache.set1 = cache.set2;
		cache.set2 = cache.set3;
		cache.set3 = SetList();
	}
}

VkDescriptorSet DescriptorSetMgr::AllocateDescriptorSet(VkDescriptorSetLayout layout)
{
	VkDescriptorSet descriptorSet;

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &layout;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &descriptorSetAllocateInfo, &descriptorSet));

	m_SetToLayout[descriptorSet] = layout;

	return descriptorSet;
}

VkDescriptorType DescriptorSetMgr::GetBindingType(VkDescriptorSet set, uint32_t binding)
{
	return m_LayoutBindingTypes[m_SetToLayout[set]][binding];
}