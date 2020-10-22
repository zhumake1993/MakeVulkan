#include "Shader.h"
#include "Tools.h"
#include "VulkanDriver.h"
#include "VKShaderModule.h"
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

void Shader::SetVertexChannels(const std::vector<VertexChannel>& channels)
{
	m_VertexChannels = channels;

	// ±£Ö¤channelµÄË³Ðò
	std::sort(m_VertexChannels.begin(), m_VertexChannels.end());
}

std::vector<VertexChannel>& Shader::GetVertexChannels()
{
	return m_VertexChannels;
}

VkShaderModule Shader::GetVkShaderModuleVert()
{
	return m_ShaderModuleVert->shaderModule;
}

VkShaderModule Shader::GetVkShaderModuleFrag()
{
	return m_ShaderModuleFrag->shaderModule;
}

VKShaderModule* Shader::LoadSPV(const std::string& filename)
{
	auto& driver = GetVulkanDriver();

	return driver.CreateVKShaderModule(filename);
}
