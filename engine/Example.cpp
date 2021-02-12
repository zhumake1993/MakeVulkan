#include "Example.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "RenderNode.h"
#include "TimeManager.h"
#include "ShaderData.h"

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

	m_TimeManager = new TimeManager();
}

Example::~Example()
{
	RELEASE(m_TimeManager);

	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	for (auto p : m_ShaderContainer) { RELEASE(p); }
	for (auto p : m_MaterialContainer) { RELEASE(p); }
	for (auto p : m_RenderNodeContainer) { RELEASE(p); }

	//todo
	//RELEASE(m_DummyShader);
}

void Example::Update()
{
	m_TimeManager->Update();
}

Mesh * Example::CreateMesh(const std::string& name)
{
	Mesh* mesh = new Mesh(name);
	m_MeshContainer.push_back(mesh);

	return mesh;
}

Texture * Example::CreateTexture(const std::string& name)
{
	Texture* texture = new Texture(name);
	m_TextureContainer.push_back(texture);

	return texture;
}

Shader * Example::CreateShader(const std::string& name)
{
	Shader* shader = new Shader(name);
	m_ShaderContainer.push_back(shader);

	return shader;
}

Material * Example::CreateMaterial(const std::string& name)
{
	Material* material = new Material(name);
	m_MaterialContainer.push_back(material);

	return material;
}

RenderNode * Example::CreateRenderNode(const std::string& name)
{
	RenderNode* renderNode = new RenderNode(name);
	m_RenderNodeContainer.push_back(renderNode);

	return renderNode;
}

void Example::SetShader(Shader * shader)
{
	auto& device = GetGfxDevice();

	device.SetPass(shader->GetGpuProgram(), shader->GetRenderStatus());
}

void Example::BindMaterial(Material * material)
{
	auto& device = GetGfxDevice();

	if (material->IsDirty())
	{
		material->UpdateUniformBuffer();
		material->SetDirty(false);
	}

	GpuProgram* gpuProgram = material->GetShader()->GetGpuProgram();
	GpuParameters& gpuParameters = gpuProgram->GetGpuParameters();
	ShaderData* shaderData = material->GetShaderData();
	
	int binding = -1;
	for (auto& uniform : gpuParameters.uniformParameters)
	{
		if (uniform.name == "PerMaterial")
		{
			binding = uniform.binding;
		}
	}

	if (binding != -1)
	{
		ASSERT(shaderData->GetDataSize() > 0, "empty data");

		Buffer* buffer = material->GetUniformBuffer();
		device.BindUniformBuffer(gpuProgram, 2, binding, buffer);
	}

	for (auto& texture : gpuParameters.textureParameters)
	{
		Texture* tex = shaderData->GetTexture(texture.name);
		device.BindImage(gpuProgram, 2, texture.binding, tex->GetImage());
	}
}

void Example::DrawRenderNode(RenderNode * node)
{
	auto& device = GetGfxDevice();

	GpuProgram* gpuProgram = node->GetMaterial()->GetShader()->GetGpuProgram();
	GpuParameters& gpuParameters = gpuProgram->GetGpuParameters();

	for (auto& uniform : gpuParameters.uniformParameters)
	{
		// 暂时只支持一个PerDraw
		if (uniform.name == "PerDraw")
		{
			glm::mat4& mat = node->GetTransform().GetMatrix();
			device.BindUniformBuffer(gpuProgram, 3, uniform.binding, &mat, sizeof(glm::mat4));
		}
	}

	Mesh* mesh = node->GetMesh();
	device.DrawBuffer(mesh->GetVertexBuffer(), mesh->GetIndexBuffer(), mesh->GetIndexCount(), mesh->GetVertexDescription());
}
