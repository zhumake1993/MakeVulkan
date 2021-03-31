#include "DescriptorSetManager.h"
#include "VulkanTools.h"

DescriptorSetManager::DescriptorSetManager(VkDevice vkDevice) :
	m_Device(vkDevice)
{
	// DescriptorPool

	std::vector<VkDescriptorPoolSize> descriptorPoolSizes(11);
	descriptorPoolSizes[0] = { VK_DESCRIPTOR_TYPE_SAMPLER , 1000 };
	descriptorPoolSizes[1] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER , 1000 };
	descriptorPoolSizes[2] = { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE , 1000 };
	descriptorPoolSizes[3] = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE , 1000 };
	descriptorPoolSizes[4] = { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER , 1000 };
	descriptorPoolSizes[5] = { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER , 1000 };
	descriptorPoolSizes[6] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER , 1000 };
	descriptorPoolSizes[7] = { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER , 1000 };
	descriptorPoolSizes[8] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC , 1000 };
	descriptorPoolSizes[9] = { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC , 1000 };
	descriptorPoolSizes[10] = { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT , 1000 };

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = nullptr;
	descriptorPoolCreateInfo.flags = 0; // 不调用VK_CHECK_RESULT(vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet));
	descriptorPoolCreateInfo.maxSets = 10000;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

	VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));
}

DescriptorSetManager::~DescriptorSetManager()
{
	for (auto& pair : m_SetCache)
	{
		auto& setlist = pair.second;
		for (auto itr = setlist.begin(); itr != setlist.end(); itr++)
		{
			RELEASE(*itr);
		}
	}

	// 销毁DescriptorPool会自动销毁其中分配的Set
	vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
	m_DescriptorPool = VK_NULL_HANDLE;
}

void DescriptorSetManager::Update()
{
	m_FrameIndex++;
}

VkDescriptorSet DescriptorSetManager::AllocateDescriptorSet(VkDescriptorSetLayout layout)
{
	if (m_SetCache.find(layout) == m_SetCache.end())
	{
		m_SetCache[layout] = SetList();

		DescriptorSet* set = AllocateDescriptorSetInternal(layout);
		m_SetCache[layout].push_front(set);

		return set->descriptorSet;
	}
	else
	{
		SetList& setlist = m_SetCache[layout];

		if (setlist.back()->InUse())
		{
			DescriptorSet* set = AllocateDescriptorSetInternal(layout);
			setlist.push_front(set);

			return set->descriptorSet;
		}
		else
		{
			DescriptorSet* set = setlist.back();
			setlist.pop_back();
			setlist.push_front(set);

			return set->descriptorSet;
		}
	}
}

DescriptorSetManager::DescriptorSet* DescriptorSetManager::AllocateDescriptorSetInternal(VkDescriptorSetLayout layout)
{
	DescriptorSet* set = new DescriptorSet();
	set->Use();

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &layout;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &descriptorSetAllocateInfo, &set->descriptorSet));

	return set;
}
