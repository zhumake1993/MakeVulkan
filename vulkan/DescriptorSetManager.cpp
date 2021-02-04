#include "DescriptorSetManager.h"
#include "VulkanTools.h"

DescriptorSetManager::DescriptorSetManager(VkDevice vkDevice) :
	m_Device(vkDevice)
{
	// DescriptorPool

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
	descriptorPoolCreateInfo.flags = 0;// VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = 1000;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

	VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));

	// Global, set = 0, Layout：
	// 0：Uniform
	{
		VkDescriptorSetLayoutBinding binding;
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = 1;
		descriptorSetLayoutCreateInfo.pBindings = &binding;

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DSLGlobal));
	}

	// PerView, set = 1, Layout：
	// 0：Uniform
	{
		VkDescriptorSetLayoutBinding binding;
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = 1;
		descriptorSetLayoutCreateInfo.pBindings = &binding;

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DSLPerView));
	}
}

DescriptorSetManager::~DescriptorSetManager()
{
	for (auto itr = m_NewDescriptorSets.begin(); itr != m_NewDescriptorSets.end(); itr++)
	{
		vkFreeDescriptorSets(m_Device, m_DescriptorPool, 1, &(*itr)->descriptorSet);
		RELEASE(*itr);
	}
	m_NewDescriptorSets.clear();

	for (auto itr = m_PendingDescriptorSets.begin(); itr != m_PendingDescriptorSets.end(); itr++)
	{
		vkFreeDescriptorSets(m_Device, m_DescriptorPool, 1, &(*itr)->descriptorSet);
		RELEASE(*itr);
	}
	m_PendingDescriptorSets.clear();

	vkDestroyDescriptorSetLayout(m_Device, m_DSLPerView, nullptr);
	m_DSLPerView = VK_NULL_HANDLE;

	vkDestroyDescriptorSetLayout(m_Device, m_DSLGlobal, nullptr);
	m_DSLGlobal = VK_NULL_HANDLE;

	// 销毁DescriptorPool会自动销毁其中分配的Set
	vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
	m_DescriptorPool = VK_NULL_HANDLE;
}

void DescriptorSetManager::Update()
{
	return;
	// 找到第一个可以被销毁的set
	auto unused = m_PendingDescriptorSets.begin();
	for (; unused != m_PendingDescriptorSets.end(); unused++)
	{
		if (!(*unused)->InUse(m_FrameIndex))
		{
			break;
		}
	}

	for (auto itr = unused; itr != m_PendingDescriptorSets.end(); itr++)
	{
		//vkFreeDescriptorSets(m_Device, m_DescriptorPool, 1, &(*itr)->descriptorSet);
		RELEASE(*itr);
	}

	m_PendingDescriptorSets.erase(unused, m_PendingDescriptorSets.end());

	// 新的set放在list前部
	m_PendingDescriptorSets.splice(m_PendingDescriptorSets.begin(), m_NewDescriptorSets);
	m_NewDescriptorSets.clear();

	m_FrameIndex++;
}

VkDescriptorSetLayout DescriptorSetManager::GetDSLGlobal()
{
	return m_DSLGlobal;
}

VkDescriptorSetLayout DescriptorSetManager::GetDSLPerView()
{
	return m_DSLPerView;
}

VkDescriptorSet DescriptorSetManager::AllocateDescriptorSet(VkDescriptorSetLayout layout)
{
	//VKDescriptorSet* descriptorSet = new VKDescriptorSet(m_FrameIndex);
	//m_NewDescriptorSets.push_back(descriptorSet);

	//
	VkDescriptorSet gggggggggggg;

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &layout;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &descriptorSetAllocateInfo, &gggggggggggg));

	//vkFreeDescriptorSets(m_Device, m_DescriptorPool, 1, &gggggggggggg);
	vkResetDescriptorPool(m_Device, m_DescriptorPool,0);

	return gggggggggggg;
}