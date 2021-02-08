#include "VKGpuProgram.h"
#include "VulkanTools.h"

VKGpuProgram::VKGpuProgram(VkDevice vkDevice, GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode) :
	GpuProgram(parameters),
	m_Device(vkDevice)
{
	std::vector<VkDescriptorSetLayoutBinding> bindingsPerMaterial;
	std::vector<VkDescriptorSetLayoutBinding> bindingsPerDraw;

	for (auto& uniform : parameters.uniformParameters)
	{
		// PerMaterial只有一个uniform buffer
		if (uniform.name == "PerMaterial")
		{
			VkDescriptorSetLayoutBinding binding = {};
			binding.binding = uniform.binding;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC?
			binding.descriptorCount = 1;
			binding.stageFlags = uniform.stageFlags;
			binding.pImmutableSamplers = nullptr;

			bindingsPerMaterial.push_back(binding);
		}
		else if (uniform.name.find("PerDraw") == 0)
		{
			VkDescriptorSetLayoutBinding binding = {};
			binding.binding = uniform.binding;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC?
			binding.descriptorCount = 1;
			binding.stageFlags = uniform.stageFlags;
			binding.pImmutableSamplers = nullptr;

			bindingsPerDraw.push_back(binding);
		}
		else
		{
			LOGE("Invalid Uniform Parameter: %s", uniform.name.c_str());
		}
	}

	for (auto& texture : parameters.textureParameters)
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = texture.binding;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 1;
		binding.stageFlags = texture.stageFlags;
		binding.pImmutableSamplers = nullptr;

		bindingsPerMaterial.push_back(binding);
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = nullptr;
	descriptorSetLayoutCreateInfo.flags = 0;

	ASSERT(bindingsPerMaterial.size() == 1, "bindingsPerMaterial.size() != 1")
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindingsPerMaterial.size());
	descriptorSetLayoutCreateInfo.pBindings = bindingsPerMaterial.data();
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DSLPerMaterial));

	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindingsPerDraw.size());
	descriptorSetLayoutCreateInfo.pBindings = bindingsPerDraw.data();
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DSLPerDraw));

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
