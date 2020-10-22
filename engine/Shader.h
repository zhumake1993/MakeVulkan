#pragma once

#include "Common.h"
#include "VKTypes.h"

struct VKShaderModule;

class Shader
{

public:

	Shader();
	~Shader();

	void LoadVertSPV(const std::string& filename);
	void LoadFragSPV(const std::string& filename);

	// ����������Ƕ�shader���з��䣬�õ�����Ķ�������ṹ�����������ֶ�����
	void SetVertexChannels(const std::vector<VertexChannel>& channels);
	std::vector<VertexChannel>& GetVertexChannels();

	VkShaderModule GetVkShaderModuleVert();
	VkShaderModule GetVkShaderModuleFrag();

private:

	VKShaderModule* LoadSPV(const std::string& filename);

public:

	//

private:

	VKShaderModule* m_ShaderModuleVert;
	VKShaderModule* m_ShaderModuleFrag;

	std::vector<VertexChannel> m_VertexChannels;
};