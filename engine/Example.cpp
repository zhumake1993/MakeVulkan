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
#include "DeviceProperties.h"
#include "Settings.h"

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
	/*m_DummyShader = new Shader("DummyShader");

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

	m_DummyShader->CreateGpuProgram(parameters);*/

	

}

void Example::Release()
{


	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	for (auto p : m_ShaderContainer) { RELEASE(p); }
	for (auto p : m_MaterialContainer) { RELEASE(p); }
	for (auto p : m_RenderNodeContainer) { RELEASE(p); }

	for (auto p : m_TempAttachments) { RELEASE(p); }

	RELEASE(m_DummyShader);
}

void Example::Update()
{
	PROFILER(Example_Update);

}

void Example::UpdateAfterDraw()
{
	for (auto p : m_TempAttachments) { RELEASE(p); }
	m_TempAttachments.clear();
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

Attachment * Example::CreateAttachment(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height)
{
	if (imageTypeMask & kImageSwapChainBit)
	{
		format = GetDeviceProperties().ScFormat.format;
		width = windowWidth;
		height = windowHeight;
	}

	return new Attachment(imageTypeMask, format, width, height);
}

Attachment * Example::CreateTempAttachment(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height)
{
	Attachment* attachment = CreateAttachment(imageTypeMask, format, width, height);
	m_TempAttachments.push_back(attachment);
	return attachment;
}

void Example::BindGlobalUniformBuffer(void * data, uint64_t size)
{
	if (size > 0)
	{
		GetGfxDevice().BindUniformBuffer(m_DummyShader->GetGpuProgram(), 0, 0, data, size);
	}
}

void Example::BindPerViewUniformBuffer(void * data, uint64_t size)
{
	if (size > 0)
	{
		GetGfxDevice().BindUniformBuffer(m_DummyShader->GetGpuProgram(), 1, 0, data, size);
	}
}

void Example::SetShader(Shader * shader)
{
	auto& device = GetGfxDevice();

	device.SetPass(shader->GetGpuProgram(), shader->GetRenderState(), shader->GetSpecializationData());
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

	MaterialBindData materialBindData;
	
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

		materialBindData.uniformBufferBindings.emplace_back(binding, buffer);
	}

	for (auto& texture : gpuParameters.textureParameters)
	{
		TextureBase* tex = shaderData->GetTexture(texture.name);

		materialBindData.imageBindings.emplace_back(texture.binding, tex->GetImage());
	}

	for (auto& inputAttachment : gpuParameters.inputAttachmentParameters)
	{
		materialBindData.inputAttachmentBindings.emplace_back(inputAttachment.binding);
	}

	device.BindMaterial(gpuProgram, materialBindData);
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

			device.BindUniformBuffer(gpuProgram, 3, uniform.binding, shaderData.GetValueData(), shaderData.GetValueDataSize());
		}
	}

	Mesh* mesh = node->GetMesh();
	device.BindMeshBuffer(mesh->GetVertexBuffer(), mesh->GetIndexBuffer(), mesh->GetVertexDescription());
	device.DrawIndexed(mesh->GetIndexCount());
}

void Example::DrawBatch(std::vector<RenderNode*> nodes)
{
	if (nodes.size() == 0)return;

	auto& device = GetGfxDevice();

	DrawBatchs drawBatchs;

	GpuProgram* gpuProgram = nodes[0]->GetMaterial()->GetShader()->GetGpuProgram();
	GpuParameters& gpuParameters = gpuProgram->GetGpuParameters();

	drawBatchs.gpuProgram = gpuProgram;

	// 找到PerDraw
	GpuParameters::UniformParameter uniformPerDraw;
	for (auto& uniform : gpuParameters.uniformParameters)
	{
		if (uniform.name == "PerDraw")
		{
			uniformPerDraw = uniform;
			break;
		}
	}

	// 计算PerDraw的size
	size_t sizePerDraw = 0;
	for (auto& vp : uniformPerDraw.valueParameters)
	{
		if (vp.name == "ObjectToWorld")
		{
			sizePerDraw += sizeof(glm::mat4);
		}
		else
		{
			LOGE("not support.");
		}
	}

	// offset必须是minUniformBufferOffsetAlignment的倍数
	auto& dp = GetDeviceProperties();
	size_t minUboAlignment = dp.deviceProperties.limits.minUniformBufferOffsetAlignment;
	drawBatchs.alignedUniformSize = (sizePerDraw + minUboAlignment - 1) & ~(minUboAlignment - 1);

	// uniform buffer
	drawBatchs.uniformBinding = 0;
	drawBatchs.uniformSize = nodes.size() * drawBatchs.alignedUniformSize;
	drawBatchs.uniformData = AlignedAlloc(drawBatchs.uniformSize, drawBatchs.alignedUniformSize); // 需要么？

	// 收集数据
	char* dst = reinterpret_cast<char*>(drawBatchs.uniformData);
	for (size_t i = 0; i < nodes.size(); i++)
	{
		char* currDst = dst;
		for (auto& vp : uniformPerDraw.valueParameters)
		{
			if (vp.name == "ObjectToWorld")
			{
				glm::mat4& mat = nodes[i]->GetTransform().GetMatrix();
				memcpy(currDst, &mat, sizeof(glm::mat4));
				currDst += sizeof(glm::mat4);
			}
			else
			{
				LOGE("not support.");
			}
		}

		dst += drawBatchs.alignedUniformSize;
	}

	// DrawItem
	for (size_t i = 0; i < nodes.size(); i++)
	{
		drawBatchs.drawItems.emplace_back();
		DrawItem& drawItem = drawBatchs.drawItems.back();

		// mesh buffer
		Mesh* mesh = nodes[i]->GetMesh();

		size_t drawBufferIndex = 0;
		for (; drawBufferIndex < drawBatchs.drawBuffers.size(); drawBufferIndex++)
		{
			if (drawBatchs.drawBuffers[drawBufferIndex].vertexBuffer == mesh->GetVertexBuffer())
			{
				break;
			}
		}

		if (drawBufferIndex < drawBatchs.drawBuffers.size())
		{
			drawItem.drawBufferIndex = drawBufferIndex;
		}
		else
		{
			// new mesh
			drawBatchs.drawBuffers.emplace_back();
			DrawBuffer& drawBuffer = drawBatchs.drawBuffers.back();

			drawBuffer.vertexBuffer = mesh->GetVertexBuffer();
			drawBuffer.indexBuffer = mesh->GetIndexBuffer();
			drawBuffer.vertexDescription = mesh->GetVertexDescription();
			drawBuffer.indexType = mesh->GetIndexType();
			drawBuffer.indexCount = mesh->GetIndexCount();

			drawItem.drawBufferIndex = drawBufferIndex;
		}
	}
	
	device.DrawBatch(drawBatchs);
}

void Example::DrawInstanced(Mesh * mesh, Shader* shader, void * data, uint64_t size, uint32_t instanceCount)
{
	auto& device = GetGfxDevice();

	GpuProgram* gpuProgram = shader->GetGpuProgram();

	// 假设binding=0
	device.BindUniformBuffer(gpuProgram, 3, 0, data, size);

	device.BindMeshBuffer(mesh->GetVertexBuffer(), mesh->GetIndexBuffer(), mesh->GetVertexDescription());
	device.DrawIndexed(mesh->GetIndexCount(), instanceCount);
}