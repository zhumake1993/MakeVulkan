#include "Engine.h"

//#include "VulkanDriver.h"
//#include "DeviceProperties.h"
//
//#include "VKCommandPool.h"
//
//#include "VKCommandBuffer.h"
//#include "VKSemaphore.h"
//#include "VKFence.h"
//#include "VKFramebuffer.h"
//
//#include "VKBuffer.h"
//#include "VKImage.h"
//#include "VKSampler.h"
//
//#include "DescriptorSetMgr.h"
//
//#include "VKShaderModule.h"
//#include "VKPipeline.h"
//#include "VKRenderPass.h"
//#include "VKSpecializationConstant.h"
//
//#include "Tools.h"
//#include "InputManager.h"
//#include "Gui.h"
//#include "TimeMgr.h"
//#include "ProfilerMgr.h"
//#include "GPUProfilerMgr.h"
//
//#include "Mesh.h"
//#include "RenderNode.h"
//#include "Texture.h"
//#include "Material.h"
//#include "Shader.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::Init()
{
}

void Engine::Release()
{
}

void Engine::Update()
{
}

//void Engine::CleanUpEngine()
//{
//	auto& driver = GetVulkanDriver();
//	driver.DeviceWaitIdle();
//
//	// 先清理子类
//
//	CleanUp();
//
//	// 再清理父类
//	
//	for (auto p : m_MeshContainer) { RELEASE(p); }
//	for (auto p : m_TextureContainer) { RELEASE(p); }
//	for (auto p : m_ShaderContainer) { RELEASE(p); }
//	for (auto p : m_MaterialContainer) { RELEASE(p); }
//	for (auto p : m_RenderNodeContainer) { RELEASE(p); }
//
//	for (auto p : m_VKPipelineContainer) { RELEASE(p); }
//
//	RELEASE(m_Imgui);
//
//	RELEASE(m_VKRenderPass);
//
//	driver.ReleaseVkPipelineLayout(m_BasePipelineLayout);
//
//	RELEASE(m_Sampler);
//
//	// 清理各种mgr
//
//	ReleaseProfilerMgr();
//
//	ReleaseTimeMgr();
//
//	// 最后清理Driver
//
//	ReleaseVulkanDriver();
//}
//
//void Engine::InitEngine()
//{
//	// 先初始化Driver
//
//	CreateVulkanDriver();
//	auto& driver = GetVulkanDriver();
//
//	// 初始化各种mgr
//
//	CreateTimeMgr();
//
//	CreateProfilerMgr();
//
//	// 再初始化父类
//
//	auto& dp = GetDeviceProperties();
//
//	uint32_t minUboAlignment = static_cast<uint32_t>(dp.deviceProperties.limits.minUniformBufferOffsetAlignment);
//	m_ObjectUBAlignment = ((sizeof(ObjectUniform) + minUboAlignment - 1) / minUboAlignment) * minUboAlignment;
//
//	driver.CreateUniformBuffer("PassUniform", sizeof(PassUniform));
//	driver.CreateUniformBuffer("ObjectUniform", m_ObjectUBAlignment * MaxObjectsCount);
//
//	m_Imgui = new Imgui();
//
//	DSLBindings bindings(1);
//	bindings[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT };
//	m_DSLPassUniform = driver.CreateDescriptorSetLayout(bindings);
//	bindings[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT };
//	m_DSLObjectDUB = driver.CreateDescriptorSetLayout(bindings);
//
//	m_VKRenderPass = driver.CreateVKRenderPass(driver.GetSwapChainFormat());
//
//	m_BasePipelineLayout = driver.CreateVkPipelineLayout({ m_DSLPassUniform, m_DSLObjectDUB });
//
//	VKSamplerCI(samplerCI);
//	m_Sampler = driver.CreateVKSampler(samplerCI);
//
//	// 最后初始化子类
//
//	Init();
//}
//
//void Engine::TickEngine()
//{
//	// 顺序很重要！！！
//
//	// 更新时间
//	auto& timeMgr = GetTimeMgr();
//	timeMgr.Tick();
//
//	PROFILER(Engine_TickEngine);
//
//	// 更新游戏逻辑
//	Tick();
//
//	// 更新UI逻辑
//	m_Imgui->Prepare();
//	TickUI();
//	ImGui::Render();
//
//	// 更新输入
//	input.Tick();
//
//	// 更新UI顶点计算
//	m_Imgui->Tick();
//
//	auto& driver = GetVulkanDriver();
//
//	driver.WaitForPresent();
//
//	UpdateUniformBuffer();
//
//	auto cb = driver.GetCurrVKCommandBuffer();
//
//	cb->Begin();
//
//	driver.Tick();
//
//	// 提交draw call
//
//	//VKFramebuffer* vkFramebuffer = driver.RebuildFramebuffer(m_VKRenderPass);
//
//	auto currPassBuffer = driver.GetUniformBuffer("PassUniform");
//	auto currDescriptorSetPerDrawcall = driver.GetDescriptorSet(m_DSLPassUniform);
//	DesUpdateInfos infosPerDrawcall(1);
//	infosPerDrawcall[0].binding = 0;
//	infosPerDrawcall[0].info.buffer = { currPassBuffer->buffer,0,sizeof(PassUniform) };
//	driver.UpdateDescriptorSet(currDescriptorSetPerDrawcall, infosPerDrawcall);
//	cb->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_BasePipelineLayout, 0, currDescriptorSetPerDrawcall);
//
//	auto currObjectUniform = driver.GetUniformBuffer("ObjectUniform");
//	m_CurrDescriptorSetObjectDUB = driver.GetDescriptorSet(m_DSLObjectDUB);
//	DesUpdateInfos infosObjectDUB(1);
//	infosObjectDUB[0].binding = 0;
//	infosObjectDUB[0].info.buffer = { currObjectUniform->buffer,0,VK_WHOLE_SIZE };
//	driver.UpdateDescriptorSet(m_CurrDescriptorSetObjectDUB, infosObjectDUB);
//
//	m_CurrMaterial = nullptr;
//
//	RecordCommandBuffer(cb);
//
//	// 提交UI draw call,todo
//	//m_Imgui->RecordCommandBuffer(m_FrameResources[m_CurrFrameIndex].commandBuffer);
//
//	driver.Present();
//}
//
//Mesh * Engine::CreateMesh()
//{
//	Mesh* mesh = new Mesh();
//	m_MeshContainer.push_back(mesh);
//
//	return mesh;
//}
//
//Texture * Engine::CreateTexture()
//{
//	Texture* texture = new Texture();
//	m_TextureContainer.push_back(texture);
//
//	return texture;
//}
//
//Shader * Engine::CreateShader()
//{
//	Shader* shader = new Shader();
//	m_ShaderContainer.push_back(shader);
//
//	return shader;
//}
//
//Material * Engine::CreateMaterial(std::string name)
//{
//	Material* material = new Material(name);
//	m_MaterialContainer.push_back(material);
//
//	return material;
//}
//
//RenderNode * Engine::CreateRenderNode()
//{
//	RenderNode* renderNode = new RenderNode(static_cast<uint32_t>(m_RenderNodeContainer.size()));
//	m_RenderNodeContainer.push_back(renderNode);
//
//	return renderNode;
//}
//
//void Engine::CreateVKPipeline(Material * mat, const VertexDescription& vertexDescription)
//{
//	auto& driver = GetVulkanDriver();
//
//	PipelineCI pipelineCI;
//
//	pipelineCI.pipelineCreateInfo.renderPass = m_VKRenderPass->renderPass;
//
//	pipelineCI.pipelineCreateInfo.layout = driver.CreateVkPipelineLayout({ m_DSLPassUniform, m_DSLObjectDUB,mat->GetVkDescriptorSetLayout() });
//
//	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = mat->GetShader()->GetVkShaderModuleVert();
//	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = mat->GetShader()->GetVkShaderModuleFrag();
//
//	pipelineCI.SetVertexInputState(vertexDescription);
//
//	pipelineCI.SetSpecializationConstant(kVKShaderFragment, mat->GetShader()->GetVKSpecializationConstant());
//
//	VKPipeline* pipeline = driver.CreateVKPipeline(pipelineCI);
//
//	mat->SetVKPipeline(pipeline);
//
//	m_VKPipelineContainer.push_back(pipeline);
//}
//
//void Engine::DrawRenderNode(VKCommandBuffer * cb, RenderNode * renderNode)
//{
//	auto& driver = GetVulkanDriver();
//
//	Material* material = renderNode->GetMaterial();
//	VKPipeline* vkPipeline = material->GetVKPipeline();
//
//	if (material != m_CurrMaterial) {
//		cb->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
//
//		auto descriptorSet = driver.GetDescriptorSet(material->GetVkDescriptorSetLayout());
//
//		DesUpdateInfos infos;
//		if (material->GetUniformDataSize() > 0) {
//			DescriptorUpdateInfo info;
//			info.binding = 0;
//			info.info.buffer = { driver.GetUniformBuffer(material->GetName())->buffer,0,material->GetUniformDataSize() };
//			infos.push_back(info);
//		}
//		std::vector<Texture*> textures = material->GetTextures();
//		for (int i = 0; i < textures.size(); i++) {
//			DescriptorUpdateInfo info;
//			info.binding = i + 1;
//			info.info.image = { m_Sampler->sampler, textures[i]->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
//			infos.push_back(info);
//		}
//
//		driver.UpdateDescriptorSet(descriptorSet, infos);
//		cb->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->pipelineLayout, 2, descriptorSet);
//
//		m_CurrMaterial = material;
//	}
//
//	cb->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->pipelineLayout, 1, m_CurrDescriptorSetObjectDUB, renderNode->GetDUBIndex() * m_ObjectUBAlignment);
//	cb->BindVertexBuffer(0, renderNode->GetVertexBuffer());
//	cb->BindIndexBuffer(renderNode->GetIndexBuffer(), VK_INDEX_TYPE_UINT32);
//	cb->DrawIndexed(renderNode->GetIndexCount(), 1, 0, 0, 1);
//}
//
//void Engine::UpdateUniformBuffer()
//{
//	auto& driver = GetVulkanDriver();
//
//	// Pass Uniform
//	auto buffer = driver.GetUniformBuffer("PassUniform");
//	buffer->Copy(&m_PassUniform, 0, sizeof(PassUniform));
//
//	// Object Uniform
//	for (auto node : m_RenderNodeContainer) {
//		auto buffer = driver.GetUniformBuffer("ObjectUniform");
//
//		if (node->IsDirty()) {
//			buffer->Copy(&node->GetWorldMatrix(), m_ObjectUBAlignment * node->GetDUBIndex(), m_ObjectUBAlignment);
//			node->Clean();
//		}
//	}
//
//	// Material Uniform
//	for (auto material : m_MaterialContainer) {
//		if (material->GetUniformDataSize() == 0) {
//			continue;
//		}
//
//		auto buffer = GetVulkanDriver().GetUniformBuffer(material->GetName());
//
//		if (material->IsDirty()) {
//			buffer->Copy(material->GetUniformData(), 0, material->GetUniformDataSize());
//			material->Clean();
//		}
//	}
//}
