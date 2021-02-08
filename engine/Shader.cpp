#include "Shader.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "GpuProgram.h"

Shader::Shader(const std::string& name) :
	m_Name(name)
{
}

Shader::~Shader()
{
	m_VertCode.clear();
	m_FragCode.clear();
	RELEASE(m_GpuProgram);
}

void Shader::LoadSPV(const std::string & vertFilename, const std::string & fragFilename)
{
	m_VertCode = GetBinaryFileContents(vertFilename);
	assert(m_VertCode.size() > 0);
	
	m_FragCode = GetBinaryFileContents(fragFilename);
	assert(m_FragCode.size() > 0);
}

void Shader::CreateGpuProgram(GpuParameters & parameters)
{
	auto& device = GetGfxDevice();

	m_GpuProgram = device.CreateGpuProgram(parameters, m_VertCode, m_FragCode);

	m_VertCode.clear();
	m_FragCode.clear();
}

GpuProgram * Shader::GetGpuProgram()
{
	return m_GpuProgram;
}

void Shader::SetRenderStatus(RenderStatus & renderStatus)
{
	m_RenderStatus = renderStatus;
}

RenderStatus & Shader::GetRenderStatus()
{
	return m_RenderStatus;
}

//void Shader::SetUniformBufferDesc(UniformBufferDesc & desc)
//{
//	m_UniformBufferDesc = desc;
//}
//
//UniformBufferDesc & Shader::GetUniformBufferDesc()
//{
//	return m_UniformBufferDesc;
//}
//
//void Shader::SetTextureDesc(const std::vector<std::string>& names)
//{
//	m_TextureNames = names;
//}
//
//std::vector<std::string>& Shader::GetTextureDesc()
//{
//	return m_TextureNames;
//}




//
//void Shader::AddSpecializationConstant(int id, uint32_t value)
//{
//	if (m_SpecializationConstant == nullptr) {
//		m_SpecializationConstant = new VKSpecializationConstant();
//	}
//
//	m_SpecializationConstant->Add(id, value);
//}
//
//void Shader::SetSpecializationConstant(int id, uint32_t value)
//{
//	if (m_SpecializationConstant == nullptr) {
//		LOG("m_SpecializationConstant is nullptr");
//		assert(false);
//	}
//
//	m_SpecializationConstant->Set(id, value);
//}
//
//VKSpecializationConstant * Shader::GetVKSpecializationConstant()
//{
//	return m_SpecializationConstant;
//}
//
//VkShaderModule Shader::GetVkShaderModuleVert()
//{
//	return m_ShaderModuleVert->shaderModule;
//}
//
//VkShaderModule Shader::GetVkShaderModuleFrag()
//{
//	return m_ShaderModuleFrag->shaderModule;
//}
//
//std::unordered_map<std::string, UniformElement>& Shader::GetUniformElements()
//{
//	return m_UniformElements;
//}
//
//uint32_t Shader::GetUniformSize()
//{
//	return m_UniformSize;
//}
//
//std::unordered_map<std::string, uint32_t>& Shader::GetTextureElements()
//{
//	return m_TextureElements;
//}
//
//uint32_t Shader::GetTextureNum()
//{
//	return m_TextureNum;
//}
//
//VKShaderModule* Shader::LoadSPV(const std::string& filename)
//{
//	auto& driver = GetVulkanDriver();
//
//	return driver.CreateVKShaderModule(filename);
//}
