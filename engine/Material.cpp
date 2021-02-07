#include "Material.h"
#include "Shader.h"
#include "GpuProgram.h"
#include "Texture.h"
#include "Tools.h"

Material::Material(std::string name) :
	m_Name(name)
{
}

Material::~Material()
{
	RELEASE(m_UniformBufferData);
}

std::string Material::GetName()
{
	return m_Name;
}

void Material::SetShader(Shader * shader)
{
	m_Shader = shader;

	// 重新创建m_UniformBufferData
	RELEASE(m_UniformBufferData);
	m_UniformBufferData = nullptr;

	uint32_t perMaterialBufferSize = m_Shader->GetGpuProgram()->GetUniformBufferSize("PerMaterial");
	if (perMaterialBufferSize > 0)
	{
		m_UniformBufferData = new char[perMaterialBufferSize];
	}


	//todo
	//std::vector<std::string>& textureDesc = m_Shader->GetTextureDesc();
	//m_Textures.resize(textureDesc.size(), nullptr);

	/*if (uniformSize > 0) {
		GetVulkanDriver().CreateUniformBuffer(m_Name, uniformSize);
	}*/

	
	// DescriptorSetLayout
	/*DSLBindings bindings(2);
	bindings[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT };
	bindings[1] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texNum, VK_SHADER_STAGE_FRAGMENT_BIT };
	m_DescriptorSetLayout = GetVulkanDriver().CreateDescriptorSetLayout(bindings);*/
}

Shader * Material::GetShader()
{
	return m_Shader;
}

void Material::SetFloat(std::string name, float x)
{
	uint32_t offset = GetUniformBufferDataOffset(name, kUniformDataTypeFloat1);

	float* addr = reinterpret_cast<float*>(m_UniformBufferData + offset);
	*(addr + 0) = x;
}

void Material::SetFloat2(std::string name, float x, float y)
{
	uint32_t offset = GetUniformBufferDataOffset(name, kUniformDataTypeFloat2);

	float* addr = reinterpret_cast<float*>(m_UniformBufferData + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
}

void Material::SetFloat3(std::string name, float x, float y, float z)
{
	uint32_t offset = GetUniformBufferDataOffset(name, kUniformDataTypeFloat3);

	float* addr = reinterpret_cast<float*>(m_UniformBufferData + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
	*(addr + 2) = z;
}

void Material::SetFloat4(std::string name, float x, float y, float z, float w)
{
	uint32_t offset = GetUniformBufferDataOffset(name, kUniformDataTypeFloat4);

	float* addr = reinterpret_cast<float*>(m_UniformBufferData + offset);
	*(addr + 0) = x;
	*(addr + 1) = y;
	*(addr + 2) = z;
	*(addr + 3) = w;
}

void Material::SetFloat4x4(std::string name, glm::mat4 & mat)
{
	uint32_t offset = GetUniformBufferDataOffset(name, kUniformDataTypeFloat4x4);

	char* addr = m_UniformBufferData + offset;
	memcpy(addr, &mat, sizeof(glm::mat4));
}

void Material::SetTextures(std::string name, Texture * texture)
{
	m_Textures.push_back(texture);
	//todo
	/*std::vector<std::string>& textureDesc = m_Shader->GetTextureDesc();
	int index = 0;
	for (; index < textureDesc.size(); index++)
	{
		if (textureDesc[index] == name)
		{
			break;
		}
	}

	if (index == textureDesc.size())
	{
		LOG("the shader of material(%s) does not have texture(%s)", m_Name.c_str(), name.c_str());
		EXIT;
	}

	m_Textures[index] = texture;*/
}

uint32_t Material::GetUniformBufferDataOffset(std::string name, UniformDataType type)
{
	for (auto& layout : m_Shader->GetGpuProgram()->GetGpuParameters().uniformBufferLayouts)
	{
		if (layout.name == "PerMaterial")
		{
			for (auto& element : layout.elements)
			{
				if (element.name == name)
				{
					if (element.type == type)
					{
						return element.offset;
					}
					else
					{
						LOG("the type(%s) of per material data(%s) is not %s", UniformDataTypeToString(element.type).c_str(), name.c_str(), UniformDataTypeToString(type).c_str());
						EXIT;
					}
				}
			}

			LOG("the shader of material(%s) does not have data(%s) in PerMaterial uniform buffer", m_Name.c_str(), name.c_str());
			EXIT;
		}
	}

	LOG("the shader of material(%s) does not have PerMaterial uniform buffer", m_Name.c_str());
	EXIT;
	return 0;
}

//void Material::SetVKPipeline(VKPipeline * vkPipeline)
//{
//	m_VKPipeline = vkPipeline;
//}
//
//VKPipeline * Material::GetVKPipeline()
//{
//	return m_VKPipeline;
//}

//char * Material::GetUniformData()
//{
//	return m_UniformData;
//}
//
//uint32_t Material::GetUniformDataSize()
//{
//	return m_Shader->GetUniformSize();
//}
//
std::vector<Texture*>& Material::GetTextures()
{
	return m_Textures;
}
//
//void Material::SetDirty()
//{
//	m_NumFramesDirty = FrameResourcesCount;
//}
//
//bool Material::IsDirty()
//{
//	return m_NumFramesDirty > 0;
//}
//
//void Material::Clean()
//{
//	if (m_NumFramesDirty > 0) {
//		m_NumFramesDirty--;
//	}
//}