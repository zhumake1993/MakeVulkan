#pragma once

#include "Common.h"
#include "VKTypes.h"

struct VKShaderModule;

class Shader
{

	using UniformPair = std::pair<UniformType, std::string>;

public:

	Shader();
	~Shader();

	void LoadVertSPV(const std::string& filename);
	void LoadFragSPV(const std::string& filename);

	// 成熟的做法是使用反射，这里手动设置
	// 假设数据是紧密pack的，这样方便处理
	void SetUniformLayout(std::vector<UniformPair> pairs);
	void SetTextureLayout(const std::vector<std::string> names);

	VkShaderModule GetVkShaderModuleVert();
	VkShaderModule GetVkShaderModuleFrag();

	std::unordered_map<std::string, UniformElement>& GetUniformElements();
	uint32_t GetUniformSize();

	std::unordered_map<std::string, uint32_t>& GetTextureElements();
	uint32_t GetTextureNum();

private:

	VKShaderModule* LoadSPV(const std::string& filename);

public:

	//

private:

	VKShaderModule* m_ShaderModuleVert = nullptr;
	VKShaderModule* m_ShaderModuleFrag = nullptr;

	std::unordered_map<std::string, UniformElement> m_UniformElements;
	uint32_t m_UniformSize = 0;

	std::unordered_map<std::string, uint32_t> m_TextureElements;
	uint32_t m_TextureNum = 0;
};