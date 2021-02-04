#include "Example.h"
#include "Tools.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "RenderNode.h"

Example::Example()
{
	//m_DummyShader = new Shader("DummyShader");

	//GpuParameters parameters;

	// Place the least frequently changing descriptor sets near the start of the pipeline layout, and place the descriptor sets representing the most frequently changing resources near the end. 
	// When pipelines are switched, only the descriptor set bindings that have been invalidated will need to be updated and the remainder of the descriptor set bindings will remain in place.

	//// set0存放自定义的uniform：Global   
	//// set1存放自定义的texture：PerView
	//// set2存放预定义的uniform：PerMaterial
	//// set3存放预定义的texture：PerDraw

	//// Global
	//{
	//	UniformBufferLayout layout0("Global", 0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	//	layout0.Add(UniformBufferElement(kUniformDataTypeFloat4, "Time"));
	//	parameters.uniformBufferLayouts.push_back(layout0);
	//}
	//
	//// PerView
	//{
	//	UniformBufferLayout layout1("PerView", 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	//	layout1.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "MatrixView"));
	//	layout1.Add(UniformBufferElement(kUniformDataTypeFloat4x4, "MatrixProj"));
	//	layout1.Add(UniformBufferElement(kUniformDataTypeFloat4, "EyePos"));
	//	parameters.uniformBufferLayouts.push_back(layout1);
	//}

	//m_DummyShader->CreateGpuProgram(parameters);
}

Example::~Example()
{
	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	for (auto p : m_ShaderContainer) { RELEASE(p); }
	for (auto p : m_MaterialContainer) { RELEASE(p); }
	for (auto p : m_RenderNodeContainer) { RELEASE(p); }

	//todo
	//RELEASE(m_DummyShader);
}

Mesh * Example::CreateMesh(std::string name)
{
	Mesh* mesh = new Mesh(name);
	m_MeshContainer.push_back(mesh);

	return mesh;
}

Texture * Example::CreateTexture(std::string name)
{
	Texture* texture = new Texture(name);
	m_TextureContainer.push_back(texture);

	return texture;
}

Shader * Example::CreateShader(std::string name)
{
	Shader* shader = new Shader(name);
	m_ShaderContainer.push_back(shader);

	return shader;
}

Material * Example::CreateMaterial(std::string name)
{
	Material* material = new Material(name);
	m_MaterialContainer.push_back(material);

	return material;
}

RenderNode * Example::CreateRenderNode(std::string name)
{
	RenderNode* renderNode = new RenderNode(name);
	m_RenderNodeContainer.push_back(renderNode);

	return renderNode;
}
