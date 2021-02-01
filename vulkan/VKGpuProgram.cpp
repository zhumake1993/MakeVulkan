#include "VKGpuProgram.h"
#include "VulkanTools.h"

VKGpuProgram::VKGpuProgram(VkDevice vkDevice, GpuParameters& parameters) :
	GpuProgram(parameters),
	m_Device(vkDevice)
{
	// PerView
	{
		for (auto& layout : parameters.uniformBufferLayouts)
		{
			if (layout.name == "PerView")
			{
				uint32_t num = 1; //todo
				std::vector<VkDescriptorSetLayoutBinding> layoutBindings(num);
				for (uint32_t i = 0; i < num; i++)
				{
					layoutBindings[i].binding = layout.binding;
					layoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // todo
					layoutBindings[i].descriptorCount = 1;
					layoutBindings[i].stageFlags = layout.stageFlags;
					layoutBindings[i].pImmutableSamplers = nullptr;
				}
			}
		}

		
	}

	// PerDraw
	{

	}
	auto& uniiformbuffers = parameters.uniformBufferLayouts;

	uint32_t num = static_cast<uint32_t>(uniiformbuffers.size());
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings(num);
	for (uint32_t i = 0; i < num; i++)
	{
		layoutBindings[i].binding = uniiformbuffers[i].binding;
		layoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // todo
		layoutBindings[i].descriptorCount = 1;
		layoutBindings[i].stageFlags = uniiformbuffers[i].stageFlags;
		layoutBindings[i].pImmutableSamplers = nullptr;
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout1));
}

VKGpuProgram::~VKGpuProgram()
{
	//vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout0, nullptr);
	vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout1, nullptr);
}
