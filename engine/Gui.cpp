#include "Gui.h"
#include "Tools.h"
#include "VulkanDriver.h"

#include "VulkanBuffer.h"
#include "VKImage.h"
#include "VKSampler.h"

#include "VulkanShaderModule.h"
#include "VKPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

#include "VulkanCommandBuffer.h"

#include "InputManager.h"

#include <algorithm>

Imgui::Imgui(VulkanRenderPass* renderpass)
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

	auto& driver = GetVulkanDriver();

	// font texture, use the default

	int width, height;
	unsigned char* pixels = NULL;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	uint64_t dataSize = width * height * 4 * sizeof(char);

	VKImageCI(imageCI);
	imageCI.extent.width = width;
	imageCI.extent.height = height;
	VKImageViewCI(imageViewCI);
	m_FontImage = driver.CreateVKImage(imageCI, imageViewCI);
	driver.UploadVKImage(m_FontImage, pixels, dataSize);

	// Sampler
	VKSamplerCI(samplerCI);
	m_Sampler = driver.CreateVKSampler(samplerCI);

	// Vertex buffer

	VkDeviceSize vertexBufferSize = m_MaxVertexCount * sizeof(ImDrawVert);
	m_VertexBuffer = driver.CreateVulkanBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	m_VertexBuffer->Map();

	// Index buffer

	VkDeviceSize indexBufferSize = m_MaxIndexCount * sizeof(ImDrawIdx);
	m_IndexBuffer = driver.CreateVulkanBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	m_IndexBuffer->Map();

	// DescriptorSet

	auto& descriptorSetMgr = driver.GetDescriptorSetMgr();

	DSLBindings bindings(1);
	bindings[0] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT };
	auto descriptorSetLayout = descriptorSetMgr.CreateDescriptorSetLayout(bindings);

	m_DescriptorSet = descriptorSetMgr.GetDescriptorSet(descriptorSetLayout, true);

	DesUpdateInfos infos(1);
	infos[0].binding = 0;
	infos[0].info.image = { m_Sampler->GetSampler(), m_FontImage->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	descriptorSetMgr.UpdateDescriptorSet(m_DescriptorSet, infos);

	// pipeline

	PipelineCI pipelineCI;

	m_PipelineLayout = driver.CreateVKPipelineLayout(descriptorSetLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 4);

	VulkanShaderModule* shaderVert = driver.CreateVulkanShaderModule(global::AssetPath + "shaders/imgui/shader.vert.spv");
	VulkanShaderModule* shaderFrag = driver.CreateVulkanShaderModule(global::AssetPath + "shaders/imgui/shader.frag.spv");

	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = shaderVert->m_ShaderModule;
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = shaderFrag->m_ShaderModule;

	std::vector<VkFormat> formats = { VK_FORMAT_R32G32_SFLOAT ,VK_FORMAT_R32G32_SFLOAT ,VK_FORMAT_R8G8B8A8_UNORM };
	pipelineCI.SetVertexInputState(formats);

	pipelineCI.pipelineCreateInfo.layout = m_PipelineLayout->GetLayout();
	pipelineCI.pipelineCreateInfo.renderPass = renderpass->m_RenderPass;

	pipelineCI.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineCI.rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	pipelineCI.colorBlendAttachmentState.blendEnable = VK_TRUE;
	pipelineCI.colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	pipelineCI.colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pipelineCI.colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	pipelineCI.colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pipelineCI.colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineCI.colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	pipelineCI.colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	
	pipelineCI.depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
	pipelineCI.depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;

	m_VulkanPipeline = driver.CreateVulkanPipeline(pipelineCI);

	RELEASE(shaderVert);
	RELEASE(shaderFrag);
}

Imgui::~Imgui()
{
	ImGui::DestroyContext();
	RELEASE(m_FontImage);
	RELEASE(m_Sampler);
	RELEASE(m_VertexBuffer);
	RELEASE(m_IndexBuffer);
	RELEASE(m_PipelineLayout);
	RELEASE(m_VulkanPipeline);
}

void Imgui::Prepare(float deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2(static_cast<float>(global::windowWidth), static_cast<float>(global::windowHeight));
	io.DeltaTime = deltaTime;

#if defined(_WIN32)
	io.MousePos = ImVec2(input.pos.x, input.pos.y);
	io.MouseDown[0] = input.key_MouseLeft;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	io.MousePos = ImVec2(input.pos0.x, input.pos0.y);
	io.MouseDown[0] = input.count != 0;
#endif

	ImGui::NewFrame();
}

void Imgui::Tick()
{
	ImDrawData* imDrawData = ImGui::GetDrawData();

	if (!imDrawData) return;

	if ((imDrawData->TotalVtxCount == 0) || (imDrawData->TotalIdxCount == 0)) {
		return;
	}

	uint32_t vertexOffset = 0;
	uint32_t indexOffset = 0;

	for (int n = 0; n < imDrawData->CmdListsCount; n++) {
		const ImDrawList* cmd_list = imDrawData->CmdLists[n];

		m_VertexBuffer->Copy(cmd_list->VtxBuffer.Data, vertexOffset, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		m_IndexBuffer->Copy(cmd_list->IdxBuffer.Data, indexOffset, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

		vertexOffset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
		indexOffset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
	}

	m_VertexBuffer->Flush();
	m_IndexBuffer->Flush();
}

void Imgui::RecordCommandBuffer(VulkanCommandBuffer * vulkanCommandBuffer)
{
	ImDrawData* imDrawData = ImGui::GetDrawData();

	if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) {
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	vulkanCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline);
	vulkanCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, m_DescriptorSet);
	vulkanCommandBuffer->BindVertexBuffer(0, m_VertexBuffer);
	vulkanCommandBuffer->BindIndexBuffer(m_IndexBuffer, VK_INDEX_TYPE_UINT16);

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(global::windowWidth);
	viewport.height = static_cast<float>(global::windowHeight);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vulkanCommandBuffer->SetViewport(viewport);

	// pushConstant
	float scale[2];
	scale[0] = 2.0f / imDrawData->DisplaySize.x;
	scale[1] = 2.0f / imDrawData->DisplaySize.y;
	float translate[2];
	translate[0] = -1.0f - imDrawData->DisplayPos.x * scale[0];
	translate[1] = -1.0f - imDrawData->DisplayPos.y * scale[1];
	vulkanCommandBuffer->PushConstants(m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
	vulkanCommandBuffer->PushConstants(m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);

	int vertexOffset = 0;
	int indexOffset = 0;

	for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
	{
		const ImDrawList* cmd_list = imDrawData->CmdLists[i];
		for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];

			VkRect2D scissorRect;
			scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
			scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
			scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
			scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);

			vulkanCommandBuffer->SetScissor(scissorRect);

			vulkanCommandBuffer->DrawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + indexOffset, pcmd->VtxOffset + vertexOffset, 0);
		}
		vertexOffset += cmd_list->VtxBuffer.Size;
		indexOffset += cmd_list->IdxBuffer.Size;
	}
}
