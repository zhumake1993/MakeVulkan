#include "VKGpuProgram.h"
#include "VulkanTools.h"
#include "DeviceProperties.h"

VkDescriptorSetLayout VKGpuProgram::m_DSLGlobal = VK_NULL_HANDLE;
VkDescriptorSetLayout VKGpuProgram::m_DSLPerView = VK_NULL_HANDLE;

VKGpuProgram::VKGpuProgram(VkDevice vkDevice, GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode) :
	GpuProgram(parameters),
	m_Device(vkDevice)
{
	std::vector<VkDescriptorSetLayoutBinding> bindingsPerMaterial;
	std::vector<VkDescriptorSetLayoutBinding> bindingsPerDraw;

	for (auto& uniform : parameters.uniformParameters)
	{
		if (uniform.name == "Global")
		{
			// Global, set = 0, Layout：
			// 0：Uniform

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
		else if (uniform.name == "PerView")
		{
			// PerView, set = 1, Layout：
			// 0：Uniform

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
		else if (uniform.name == "PerMaterial") // PerMaterial只有一个uniform buffer
		{
			VkDescriptorSetLayoutBinding binding = {};
			binding.binding = uniform.binding;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC?
			binding.descriptorCount = 1;
			binding.stageFlags = uniform.stageFlags;
			binding.pImmutableSamplers = nullptr;

			bindingsPerMaterial.push_back(binding);
		}
		else if (uniform.name.find("PerDraw") == 0) // 目前PerDraw只有一个uniform buffer
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

	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindingsPerMaterial.size());
	descriptorSetLayoutCreateInfo.pBindings = bindingsPerMaterial.data();
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DSLPerMaterial));

	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindingsPerDraw.size());
	descriptorSetLayoutCreateInfo.pBindings = bindingsPerDraw.data();
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DSLPerDraw));

	// PipelineLayout

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	descriptorSetLayouts.push_back(m_DSLGlobal);
	descriptorSetLayouts.push_back(m_DSLPerView);
	descriptorSetLayouts.push_back(m_DSLPerMaterial);
	descriptorSetLayouts.push_back(m_DSLPerDraw);

	VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
	pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCI.pNext = nullptr;
	pipelineLayoutCI.flags = 0;
	pipelineLayoutCI.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutCI.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutCI.pushConstantRangeCount = 0;
	pipelineLayoutCI.pPushConstantRanges = nullptr;

	if (parameters.pushConstantSize > 0)
	{
		auto& dp = GetDeviceProperties();
		ASSERT(parameters.pushConstantSize <= dp.deviceProperties.limits.maxPushConstantsSize, "exceed maxPushConstantsSize.");

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = parameters.pushConstantStage;
		pushConstantRange.offset = 0;
		pushConstantRange.size = parameters.pushConstantSize;

		// 一个PushConstant足够了
		pipelineLayoutCI.pushConstantRangeCount = 1;
		pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
	}

	VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device, &pipelineLayoutCI, nullptr, &m_PipelineLayout));

	if(vertCode.size() > 0)
	{
		VkShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.pNext = nullptr;
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.codeSize = vertCode.size();
		moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertCode.data());

		VK_CHECK_RESULT(vkCreateShaderModule(m_Device, &moduleCreateInfo, nullptr, &m_VertShaderModule));
	}
	
	if(fragCode.size() > 0)
	{
		VkShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.pNext = nullptr;
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.codeSize = fragCode.size();
		moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragCode.data());

		VK_CHECK_RESULT(vkCreateShaderModule(m_Device, &moduleCreateInfo, nullptr, &m_FragShaderModule));
	}

	// SpecializationConstant

	uint32_t offset = 0;
	for (auto& sc : parameters.SCParameters)
	{
		VkSpecializationMapEntry entry = {};
		entry.constantID = sc.id;
		entry.size = ShaderDataTypeToSize(sc.type);
		entry.offset = offset;

		offset += static_cast<uint32_t>(entry.size);

		m_SpecializationMapEntries.push_back(entry);
	}

	m_SpecializationInfo.dataSize = offset;
	m_SpecializationInfo.mapEntryCount = static_cast<uint32_t>(m_SpecializationMapEntries.size());
	m_SpecializationInfo.pMapEntries = m_SpecializationMapEntries.data();
	m_SpecializationInfo.pData = nullptr;
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

	vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
	m_PipelineLayout = VK_NULL_HANDLE;
}

VkShaderModule VKGpuProgram::GetVertShaderModule()
{
	return m_VertShaderModule;
}

VkShaderModule VKGpuProgram::GetFragShaderModule()
{
	return m_FragShaderModule;
}

VkDescriptorSetLayout VKGpuProgram::GetDSLGlobal()
{
	return m_DSLGlobal;
}

VkDescriptorSetLayout VKGpuProgram::GetDSLPerView()
{
	return m_DSLPerView;
}

VkDescriptorSetLayout VKGpuProgram::GetDSLPerMaterial()
{
	return m_DSLPerMaterial;
}

VkDescriptorSetLayout VKGpuProgram::GetDSLPerDraw()
{
	return m_DSLPerDraw;
}

VkPipelineLayout VKGpuProgram::GetPipelineLayout()
{
	return m_PipelineLayout;
}

VkSpecializationInfo & VKGpuProgram::GetSpecializationInfo()
{
	return m_SpecializationInfo;
}
