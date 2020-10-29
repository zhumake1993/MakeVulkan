#include "Engine.h"

#include "VulkanDriver.h"
#include "DeviceProperties.h"

#include "VKCommandPool.h"

#include "VKCommandBuffer.h"
#include "VKSemaphore.h"
#include "VKFence.h"
#include "VKFramebuffer.h"

#include "VKBuffer.h"
#include "VKImage.h"

#include "DescriptorSetMgr.h"

#include "VKShaderModule.h"
#include "VKPipelineLayout.h"
#include "VKPipeline.h"
#include "VKRenderPass.h"

#include "Tools.h"
#include "InputManager.h"
#include "Gui.h"
#include "TimeMgr.h"
#include "ProfilerMgr.h"
#include "GPUProfilerMgr.h"

#include "Mesh.h"
#include "RenderNode.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::CleanUpEngine()
{
	auto& driver = GetVulkanDriver();
	driver.DeviceWaitIdle();

	// ����������

	CleanUp();

	// ��������

	for (auto p : m_MeshContainer) { RELEASE(p); }
	for (auto p : m_TextureContainer) { RELEASE(p); }
	for (auto p : m_ShaderContainer) { RELEASE(p); }
	for (auto p : m_MaterialContainer) { RELEASE(p); }
	for (auto p : m_RenderNodeContainer) { RELEASE(p); }

	RELEASE(m_Imgui);

	// �������mgr

	GetProfilerMgr().WriteToFile();

	ReleaseProfilerMgr();

	ReleaseTimeMgr();

	// �������Driver

	ReleaseVulkanDriver();
}

void Engine::InitEngine()
{
	// �ȳ�ʼ��Driver

	CreateVulkanDriver();
	auto& driver = GetVulkanDriver();

	// ��ʼ������mgr

	CreateTimeMgr();

	CreateProfilerMgr();

	// �ٳ�ʼ������

	auto& dp = GetDeviceProperties();

	uint32_t minUboAlignment = static_cast<uint32_t>(dp.deviceProperties.limits.minUniformBufferOffsetAlignment);
	m_ObjectUBAlignment = ((sizeof(ObjectUniform) + minUboAlignment - 1) / minUboAlignment) * minUboAlignment;
	m_MaterialUBAlignment = ((sizeof(MaterialUniform) + minUboAlignment - 1) / minUboAlignment) * minUboAlignment;

	driver.CreateUniformBuffer("PassUniform", sizeof(PassUniform));
	driver.CreateUniformBuffer("ObjectUniform", m_ObjectUBAlignment * MaxObjectsCount);
	driver.CreateUniformBuffer("MaterialUniform", m_MaterialUBAlignment * MaxMaterialsCount);

	m_Imgui = new Imgui();

	// ����ʼ������

	Init();
}

void Engine::TickEngine()
{
	// ˳�����Ҫ������

	// ����ʱ��
	auto& timeMgr = GetTimeMgr();
	timeMgr.Tick();

	PROFILER(Engine_TickEngine);

	// ������Ϸ�߼�
	Tick();

	// ����UI�߼�
	m_Imgui->Prepare();
	TickUI();
	ImGui::Render();

	// ��������
	input.Tick();

	// ����UI�������
	m_Imgui->Tick();

	auto& driver = GetVulkanDriver();

	driver.WaitForPresent();

	UpdatePassUniformBuffer(&m_PassUniform);

	for (auto node : m_RenderNodeContainer) {
		UpdateObjectUniformBuffer(node);
	}
	for (auto mat : m_MaterialContainer) {
		UpdateMaterialUniformBuffer(mat);
	}

	driver.Tick();

	// �ύdraw call
	RecordCommandBuffer(driver.GetCurrVKCommandBuffer());

	// �ύUI draw call,todo
	//m_Imgui->RecordCommandBuffer(m_FrameResources[m_CurrFrameIndex].commandBuffer);

	driver.Present();
}

void Engine::UpdatePassUniformBuffer(void * data)
{
	auto buffer = GetVulkanDriver().GetUniformBuffer("PassUniform");

	buffer->Copy(data, 0, sizeof(PassUniform));
}

void Engine::UpdateObjectUniformBuffer(RenderNode* node)
{
	auto& driver = GetVulkanDriver();

	auto buffer = driver.GetUniformBuffer("ObjectUniform");

	if (node->IsDirty()) {
		buffer->Copy(&node->GetWorldMatrix(), m_ObjectUBAlignment * node->GetDUBIndex(), m_ObjectUBAlignment);
		node->Clean();
	}
}

void Engine::UpdateMaterialUniformBuffer(Material * material)
{
	auto& driver = GetVulkanDriver();

	auto buffer = driver.GetUniformBuffer("MaterialUniform");

	MaterialUniform matUniform;
	matUniform.diffuseAlbedo = material->GetDiffuseAlbedo();
	matUniform.fresnelR0 = material->GetFresnelR0();
	matUniform.roughness = material->GetRoughness();
	matUniform.matTransform = material->GetMatTransform();

	if (material->IsDirty()) {
		buffer->Copy(&matUniform, m_MaterialUBAlignment * material->GetDUBIndex(), m_MaterialUBAlignment);
		material->Clean();
	}
}

Mesh * Engine::CreateMesh()
{
	Mesh* mesh = new Mesh();
	m_MeshContainer.push_back(mesh);

	return mesh;
}

Texture * Engine::CreateTexture()
{
	Texture* texture = new Texture();
	m_TextureContainer.push_back(texture);

	return texture;
}

Shader * Engine::CreateShader()
{
	Shader* shader = new Shader();
	m_ShaderContainer.push_back(shader);

	return shader;
}

Material * Engine::CreateMaterial()
{
	Material* material = new Material(static_cast<uint32_t>(m_MaterialContainer.size()));
	m_MaterialContainer.push_back(material);

	return material;
}

RenderNode * Engine::CreateRenderNode()
{
	RenderNode* renderNode = new RenderNode(static_cast<uint32_t>(m_RenderNodeContainer.size()));
	m_RenderNodeContainer.push_back(renderNode);

	return renderNode;
}