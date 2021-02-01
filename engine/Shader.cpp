#include "Shader.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "GpuProgram.h"

Shader::Shader(std::string name) :
	m_Name(name)
{
}

Shader::~Shader()
{
	//RELEASE(m_ShaderModuleVert);
	//RELEASE(m_ShaderModuleFrag);
	RELEASE(m_GpuProgram);
}

void Shader::LoadSPV(const std::string & vertFilename, const std::string & fragFilename)
{
	auto& device = GetGfxDevice();

	//VKShaderModule* m_ShaderModuleVert = device.CreateVKShaderModule(vertFilename);
	//VKShaderModule* m_ShaderModuleFrag = device.CreateVKShaderModule(fragFilename);
}

void Shader::CreateGpuProgram(GpuParameters & parameters)
{
	auto& device = GetGfxDevice();

	m_GpuProgram = device.CreateGpuProgram(parameters);
}

GpuProgram & Shader::GetGpuProgram()
{
	return *m_GpuProgram;
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
