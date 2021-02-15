#include "Example.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "RenderNode.h"
#include "TimeManager.h"
#include "ProfilerManager.h"
#include "ShaderData.h"
#include "Imgui.h"

// Place the least frequently changing descriptor sets near the start of the pipeline layout, and place the descriptor sets representing the most frequently changing resources near the end. 
// When pipelines are switched, only the descriptor set bindings that have been invalidated will need to be updated and the remainder of the descriptor set bindings will remain in place.
// set0存放自定义的uniform：Global   
// set1存放自定义的texture：PerView
// set2存放预定义的uniform：PerMaterial
// set3存放预定义的texture：PerDraw

Example::Example()
{
}

Example::~Example()
{
}

void Example::Init()
{
	m_DummyShader = new Shader("DummyShader");

	GpuParameters parameters;
	{
		GpuParameters::UniformParameter uniform("Global", 0, VK_SHADER_STAGE_VERTEX_BIT);
		uniform.valueParameters.emplace_back("Time", kShaderDataFloat4);
		parameters.uniformParameters.push_back(uniform);
	}
	{
		GpuParameters::UniformParameter uniform("PerView", 0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		uniform.valueParameters.emplace_back("MatrixView", kShaderDataFloat4x4);
		uniform.valueParameters.emplace_back("MatrixProj", kShaderDataFloat4x4);
		uniform.valueParameters.emplace_back("EyePos", kShaderDataFloat4);
		parameters.uniformParameters.push_back(uniform);
	}

	m_DummyShader->CreateGpuProgram(parameters);

	m_TimeManager = new TimeManager();

	m_Imgui = new Imgui();
}

void Example::Release()
{
	RELEASE(m_Imgui);
	RELEASE(m_TimeManager);

	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	for (auto p : m_ShaderContainer) { RELEASE(p); }
	for (auto p : m_MaterialContainer) { RELEASE(p); }
	for (auto p : m_RenderNodeContainer) { RELEASE(p); }

	RELEASE(m_DummyShader);
}

void Example::Update()
{
	PROFILER(Example_Update);

	m_TimeManager->Update();

	m_Imgui->Prepare(m_TimeManager->GetDeltaTime());

	SetShader(m_DummyShader);
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
	PROFILER(Example_SetShader);

	auto& device = GetGfxDevice();

	device.SetPass(shader->GetGpuProgram(), shader->GetRenderState(), shader->GetSpecializationData());
}

void Example::BindGlobalUniformBuffer()
{
	ShaderBindings shaderBindings;
	shaderBindings.uniformDataBindings.emplace_back(0, &m_UniformDataGlobal, sizeof(UniformDataGlobal));
	GetGfxDevice().BindShaderResources(m_DummyShader->GetGpuProgram(), 0, shaderBindings);
}

void Example::BindPerViewUniformBuffer()
{
	ShaderBindings shaderBindings;
	shaderBindings.uniformDataBindings.emplace_back(0, &m_UniformDataPerView, sizeof(UniformDataPerView));
	GetGfxDevice().BindShaderResources(m_DummyShader->GetGpuProgram(), 1, shaderBindings);
}

void Example::BindMaterial(Material * material)
{
	PROFILER(Example_BindMaterial);

	auto& device = GetGfxDevice();

	if (material->IsDirty())
	{
		material->UpdateUniformBuffer();
		material->SetDirty(false);
	}

	GpuProgram* gpuProgram = material->GetShader()->GetGpuProgram();
	GpuParameters& gpuParameters = gpuProgram->GetGpuParameters();
	ShaderData* shaderData = material->GetShaderData();

	ShaderBindings shaderBindings;
	
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
		ASSERT(shaderData->GetValueDataSize() > 0, "empty data");

		Buffer* buffer = material->GetUniformBuffer();

		shaderBindings.uniformBufferBindings.emplace_back(binding, buffer);
	}

	for (auto& texture : gpuParameters.textureParameters)
	{
		Texture* tex = shaderData->GetTexture(texture.name);

		shaderBindings.imageBindings.emplace_back(texture.binding, tex->GetImage());
	}

	device.BindShaderResources(gpuProgram, 2, shaderBindings);
}

void Example::DrawRenderNode(RenderNode * node)
{
	PROFILER(Example_DrawRenderNode);

	auto& device = GetGfxDevice();

	GpuProgram* gpuProgram = node->GetMaterial()->GetShader()->GetGpuProgram();
	GpuParameters& gpuParameters = gpuProgram->GetGpuParameters();

	for (auto& uniform : gpuParameters.uniformParameters)
	{
		// 暂时只支持一个PerDraw
		if (uniform.name == "PerDraw")
		{
			ShaderData shaderData;
			shaderData.SetValueParameter(uniform.valueParameters);

			for (auto& vp : uniform.valueParameters)
			{
				if (vp.name == "ObjectToWorld")
				{
					glm::mat4& mat = node->GetTransform().GetMatrix();
					shaderData.SetFloat4x4("ObjectToWorld", mat);
				}
				else
				{
					LOGE("not support.");
				}
			}

			ShaderBindings shaderBindings;
			shaderBindings.uniformDataBindings.emplace_back(uniform.binding, shaderData.GetValueData(), shaderData.GetValueDataSize());
			device.BindShaderResources(gpuProgram, 3, shaderBindings);
		}
	}

	PROFILER(BindMeshBuffer);

	Mesh* mesh = node->GetMesh();
	device.BindMeshBuffer(mesh->GetVertexBuffer(), mesh->GetIndexBuffer(), mesh->GetVertexDescription());
	device.DrawIndexed(mesh->GetIndexCount());
}

void Example::UpdateImgui()
{
	ImGui::Render();
	m_Imgui->Tick();
}

void Example::DrawImgui()
{
	m_Imgui->Draw();
}
