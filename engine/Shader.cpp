#include "Shader.h"
#include "Tools.h"
#include "VulkanDriver.h"
#include "VKShaderModule.h"
#include "VKSpecializationConstant.h"
#include <algorithm>

Shader::Shader()
{
}

Shader::~Shader()
{
	RELEASE(m_ShaderModuleVert);
	RELEASE(m_ShaderModuleFrag);
}

void Shader::LoadVertSPV(const std::string& filename)
{
	m_ShaderModuleVert = LoadSPV(filename);
}

void Shader::LoadFragSPV(const std::string& filename)
{
	m_ShaderModuleFrag = LoadSPV(filename);
}

void Shader::SetUniformLayout(std::vector<UniformPair> pairs)
{
	uint32_t num = static_cast<uint32_t>(pairs.size());

	uint32_t offset = 0;
	for (uint32_t i = 0; i < num; i++) {
		auto uniformElement = UniformElement();
		uniformElement.offset = offset;
		uniformElement.type = pairs[i].first;

		m_UniformElements[pairs[i].second] = uniformElement;

		offset += UniformTypeToSize(pairs[i].first);
	}

	m_UniformSize = offset;
}

void Shader::SetTextureLayout(const std::vector<std::string> names)
{
	m_TextureNum = static_cast<uint32_t>(names.size());

	for (uint32_t i = 0; i < m_TextureNum; i++) {
		m_TextureElements[names[i]] = i;
	}
}

void Shader::AddSpecializationConstant(int id, uint32_t value)
{
	if (m_SpecializationConstant == nullptr) {
		m_SpecializationConstant = new VKSpecializationConstant();
	}

	m_SpecializationConstant->Add(id, value);
}

void Shader::SetSpecializationConstant(int id, uint32_t value)
{
	if (m_SpecializationConstant == nullptr) {
		LOG("m_SpecializationConstant is nullptr");
		assert(false);
	}

	m_SpecializationConstant->Set(id, value);
}

VKSpecializationConstant * Shader::GetVKSpecializationConstant()
{
	return m_SpecializationConstant;
}

VkShaderModule Shader::GetVkShaderModuleVert()
{
	return m_ShaderModuleVert->shaderModule;
}

VkShaderModule Shader::GetVkShaderModuleFrag()
{
	return m_ShaderModuleFrag->shaderModule;
}

std::unordered_map<std::string, UniformElement>& Shader::GetUniformElements()
{
	return m_UniformElements;
}

uint32_t Shader::GetUniformSize()
{
	return m_UniformSize;
}

std::unordered_map<std::string, uint32_t>& Shader::GetTextureElements()
{
	return m_TextureElements;
}

uint32_t Shader::GetTextureNum()
{
	return m_TextureNum;
}

VKShaderModule* Shader::LoadSPV(const std::string& filename)
{
	auto& driver = GetVulkanDriver();

	return driver.CreateVKShaderModule(filename);
}
