#include "Gui.h"
#include "Tools.h"
#include "VulkanDriver.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

#include "VulkanCommandBuffer.h"

#include <algorithm>

// test!!
#include "VulkanDevice.h"

Imgui::Imgui(VulkanRenderPass* renderpass)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	auto& driver = GetVulkanDriver();

	// texture

	int width, height;
	unsigned char* pixels = NULL;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	VkDeviceSize dataSize = width * height * 4 * sizeof(char);

	m_FontImage = driver.CreateVulkanImage(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	driver.UploadVulkanImage(m_FontImage, pixels, dataSize);

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
	infos[0].info.image = { m_FontImage->m_Sampler, m_FontImage->m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	descriptorSetMgr.UpdateDescriptorSet(m_DescriptorSet, infos);

	// pipeline

	PipelineCI pipelineCI;

	//m_VulkanPipelineLayout = driver.CreateVulkanPipelineLayout(descriptorSetLayout);
	// Push constants for UI rendering parameters
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstBlock);
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
	VK_CHECK_RESULT(vkCreatePipelineLayout(driver.GetVulkanDevice()->m_LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));

	VulkanShaderModule* shaderVert = driver.CreateVulkanShaderModule(global::AssetPath + "shaders/imgui/shader.vert.spv");
	VulkanShaderModule* shaderFrag = driver.CreateVulkanShaderModule(global::AssetPath + "shaders/imgui/shader.frag.spv");

	pipelineCI.shaderStageCreateInfos[kVKShaderVertex].module = shaderVert->m_ShaderModule;
	pipelineCI.shaderStageCreateInfos[kVKShaderFragment].module = shaderFrag->m_ShaderModule;

	std::vector<VkFormat> formats = { VK_FORMAT_R32G32_SFLOAT ,VK_FORMAT_R32G32_SFLOAT ,VK_FORMAT_R8G8B8A8_UNORM };
	pipelineCI.SetVertexInputState(formats);

	// test
	pipelineCI.pipelineCreateInfo.layout = m_PipelineLayout;
	pipelineCI.pipelineCreateInfo.renderPass = renderpass->m_RenderPass;

	pipelineCI.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineCI.rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	pipelineCI.colorBlendAttachmentState.blendEnable = VK_TRUE;
	pipelineCI.colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	pipelineCI.colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	pipelineCI.colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pipelineCI.colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	pipelineCI.colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pipelineCI.colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineCI.colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	
	pipelineCI.depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
	pipelineCI.depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;

	m_VulkanPipeline = driver.CreateVulkanPipeline(pipelineCI);

	RELEASE(shaderVert);
	RELEASE(shaderFrag);
}

Imgui::~Imgui()
{
	RELEASE(m_FontImage);
	RELEASE(m_VertexBuffer);
	RELEASE(m_IndexBuffer);
	RELEASE(m_VulkanPipelineLayout);
	RELEASE(m_VulkanPipeline);
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
	//vulkanCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipelineLayout, m_DescriptorSet);
	// test
	vkCmdBindDescriptorSets(vulkanCommandBuffer->m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
	vulkanCommandBuffer->BindVertexBuffer(0, m_VertexBuffer);
	vulkanCommandBuffer->BindIndexBuffer(m_IndexBuffer, VK_INDEX_TYPE_UINT16);

	// test
	PushConstBlock pushConstBlock;
	pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	pushConstBlock.translate = glm::vec2(-1.0f);
	// test
	vkCmdPushConstants(vulkanCommandBuffer->m_CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &pushConstBlock);

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

			//indexOffset += pcmd->ElemCount;
		}
		vertexOffset += cmd_list->VtxBuffer.Size;
		indexOffset += cmd_list->IdxBuffer.Size;
	}
}
