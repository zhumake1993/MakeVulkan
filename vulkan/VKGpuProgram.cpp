#include "VKGpuProgram.h"
#include "VulkanTools.h"

VKGpuProgram::VKGpuProgram(VkDevice vkDevice, GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode) :
	GpuProgram(parameters),
	m_Device(vkDevice)
{
	for (auto& layout : parameters.uniformBufferLayouts)
	{
		if (layout.name == "PerMaterial")//todo
		{
			uint32_t num = 1;
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings(num);
			for (uint32_t i = 0; i < num; i++)
			{
				layoutBindings[i].binding = layout.binding;
				layoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBindings[i].descriptorCount = 1;
				layoutBindings[i].stageFlags = layout.stageFlags;
				layoutBindings[i].pImmutableSamplers = nullptr;
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DSLPerMaterial));
		}
		else if (layout.name == "PerDraw")//todo
		{
			uint32_t num = 1;
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings(num);
			for (uint32_t i = 0; i < num; i++)
			{
				layoutBindings[i].binding = layout.binding;
				layoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBindings[i].descriptorCount = 1;
				layoutBindings[i].stageFlags = layout.stageFlags;
				layoutBindings[i].pImmutableSamplers = nullptr;
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DSLPerDraw));
		}
	}

	ASSERT(m_DSLPerMaterial != VK_NULL_HANDLE, "m_DSLPerMaterial == VK_NULL_HANDLE");
	ASSERT(m_DSLPerDraw != VK_NULL_HANDLE, "m_DSLPerDraw == VK_NULL_HANDLE");

	{
		VkShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.pNext = nullptr;
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.codeSize = vertCode.size();
		moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertCode.data());

		VK_CHECK_RESULT(vkCreateShaderModule(m_Device, &moduleCreateInfo, nullptr, &m_VertShaderModule));
	}
	
	{
		VkShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.pNext = nullptr;
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.codeSize = fragCode.size();
		moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragCode.data());

		VK_CHECK_RESULT(vkCreateShaderModule(m_Device, &moduleCreateInfo, nullptr, &m_FragShaderModule));
	}
}

VKGpuProgram::~VKGpuProgram()
{
	vkDestroyShaderModule(m_Device, m_VertShaderModule, nullptr);
	m_VertShaderModule = VK_NULL_HANDLE;

	vkDestroyShaderModule(m_Device, m_FragShaderModule, nullptr);
	m_FragShaderModule = VK_NULL_HANDLE;

	vkDestroyDescriptorSetLayout(m_Device, m_DSLPerMaterial, nullptr);
	m_DSLPerMaterial = VK_NULL_HANDLE;

	vkDestroyDescriptorSetLayout(m_Device, m_DSLPerDraw, nullptr);
	m_DSLPerDraw = VK_NULL_HANDLE;
}

VkShaderModule VKGpuProgram::GetVertShaderModule()
{
	return m_VertShaderModule;
}

VkShaderModule VKGpuProgram::GetFragShaderModule()
{
	return m_FragShaderModule;
}

VkDescriptorSetLayout& VKGpuProgram::GetDSLPerMaterial()
{
	return m_DSLPerMaterial;
}

VkDescriptorSetLayout& VKGpuProgram::GetDSLPerDraw()
{
	return m_DSLPerDraw;
}
