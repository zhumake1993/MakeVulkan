#pragma once

#include "Env.h"
#include "GfxTypes.h"

class GpuProgram;

class Shader
{

public:

	Shader(std::string name);
	~Shader();

	void LoadSPV(const std::string& vertFilename, const std::string& fragFilename);

	// 成熟的做法是使用反射，这里手动设置
	// 假设数据是紧密pack的（满足对齐要求），这样方便处理
	void CreateGpuProgram(GpuParameters& parameters);
	GpuProgram* GetGpuProgram();

	void SetRenderStatus(RenderStatus& renderStatus);
	RenderStatus& GetRenderStatus();

	//void SetUniformBufferDesc(UniformBufferDesc& desc);
	//UniformBufferDesc& GetUniformBufferDesc();

	//void SetTextureDesc(const std::vector<std::string>& names);
	//std::vector<std::string>& GetTextureDesc();

	//void AddSpecializationConstant(int id, uint32_t value);
	//void SetSpecializationConstant(int id, uint32_t value);
	//VKSpecializationConstant* GetVKSpecializationConstant();

	//VkShaderModule GetVkShaderModuleVert();
	//VkShaderModule GetVkShaderModuleFrag();

	//std::unordered_map<std::string, UniformElement>& GetUniformElements();
	//uint32_t GetUniformSize();

	//std::unordered_map<std::string, uint32_t>& GetTextureElements();
	//uint32_t GetTextureNum();

public:

	//

private:

	std::string m_Name;

	std::vector<char> m_VertCode;
	std::vector<char> m_FragCode;

	GpuProgram* m_GpuProgram;

	RenderStatus m_RenderStatus;

	//UniformBufferDesc m_UniformBufferDesc;

	//std::vector<std::string> m_TextureNames;

	//VKSpecializationConstant* m_SpecializationConstant = nullptr;
};