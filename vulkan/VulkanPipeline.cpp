#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanRenderPass.h"
#include "Tools.h"

void PipelineCI::Configure(VulkanPipelineLayout* vulkanPipelineLayout, VulkanRenderPass* vulkanRenderPass)
{
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = nullptr;
	pipelineCreateInfo.flags = 0;

	ConfigShaderStageCreateInfos();
	ConfigVertexInputStateCreateInfo();
	ConfigInputAssemblyStateCreateInfo();
	ConfigTessellationStateCreateInfo();
	ConfigViewportStateCreateInfo();
	ConfigRasterizationStateCreateInfo();
	ConfigMultisampleStateCreateInfo();
	ConfigDepthStencilStateCreateInfo();
	ConfigColorBlendStateCreateInfo();
	ConfigDynamicStateCreateInfo();

	pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	pipelineCreateInfo.layout = vulkanPipelineLayout->m_PipelineLayout;
	pipelineCreateInfo.renderPass = vulkanRenderPass->m_RenderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;
}

void PipelineCI::ConfigShaderStageCreateInfos()
{
	// Vertex shader
	shaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfos[0].pNext = nullptr;
	shaderStageCreateInfos[0].flags = 0;
	shaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageCreateInfos[0].module = shaderStage.vertShaderModule->m_ShaderModule;
	shaderStageCreateInfos[0].pName = shaderStage.vertEntry.c_str();
	shaderStageCreateInfos[0].pSpecializationInfo = nullptr;

	// Fragment shader
	shaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfos[1].pNext = nullptr;
	shaderStageCreateInfos[1].flags = 0;
	shaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStageCreateInfos[1].module = shaderStage.fragShaderModule->m_ShaderModule;
	shaderStageCreateInfos[1].pName = shaderStage.fragEntry.c_str();
	shaderStageCreateInfos[1].pSpecializationInfo = nullptr;

	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStageCreateInfos;
}

void PipelineCI::ConfigVertexInputStateCreateInfo()
{
	// Vertex input binding
	// This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
	vertexInputBindings[0].binding = 0;
	vertexInputBindings[0].stride = vertexInputState.GetStride();
	vertexInputBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// Inpute attribute bindings describe shader attribute locations and memory layouts
	uint32_t num = static_cast<uint32_t>(vertexInputState.vertexLayout.size());
	uint32_t offset = 0;
	for (uint32_t i = 0; i < num; i++) {
		vertexInputAttributs[i].binding = 0;
		vertexInputAttributs[i].location = i;
		vertexInputAttributs[i].format = vertexInputState.GetVkFormat(i);
		vertexInputAttributs[i].offset = offset;
		offset += vertexInputState.GetSize(i);
	}

	// Vertex input state used for pipeline creation
	vertexInputStateCreateInfo.pNext = nullptr;
	vertexInputStateCreateInfo.flags = 0;
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(1);
	vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindings;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = num;
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributs;

	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
}

void PipelineCI::ConfigInputAssemblyStateCreateInfo()
{
	// Input assembly state describes how primitives are assembled
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.pNext = nullptr;
	inputAssemblyStateCreateInfo.flags = 0;
	inputAssemblyStateCreateInfo.topology = inputAssemblyState.primitiveTopology;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
}

void PipelineCI::ConfigTessellationStateCreateInfo()
{
	pipelineCreateInfo.pTessellationState = &tessellationStateCreateInfo;
}

void PipelineCI::ConfigViewportStateCreateInfo()
{
	// Viewport state sets the number of viewports and scissor used in this pipeline
	// Note: This is actually overriden by the dynamic states (see below)
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.pNext = nullptr;
	viewportStateCreateInfo.flags = 0;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = nullptr;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = nullptr;

	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
}

void PipelineCI::ConfigRasterizationStateCreateInfo()
{
	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.pNext = nullptr;
	rasterizationStateCreateInfo.flags = 0;
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateCreateInfo.cullMode = rasterizationState.cullMode;
	rasterizationStateCreateInfo.frontFace = rasterizationState.frontFace;
	rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	rasterizationStateCreateInfo.lineWidth = 1.0f;

	pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
}

void PipelineCI::ConfigMultisampleStateCreateInfo()
{
	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateCreateInfo.pNext = nullptr;
	multisampleStateCreateInfo.flags = 0;
	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.minSampleShading = 1.0f;
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
}

void PipelineCI::ConfigDepthStencilStateCreateInfo()
{
	depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateCreateInfo.pNext = nullptr;
	depthStencilStateCreateInfo.flags = 0;

	depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
	depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
	depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;

	depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	depthStencilStateCreateInfo.front = {};
	depthStencilStateCreateInfo.back.failOp = {};

	depthStencilStateCreateInfo.minDepthBounds = 0.0f;
	depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

	pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
}

void PipelineCI::ConfigColorBlendStateCreateInfo()
{
	// Color blend state describes how blend factors are calculated (if used)
	// We need one blend attachment state per color attachment (even if blending is not used)
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateCreateInfo.pNext = nullptr;
	colorBlendStateCreateInfo.flags = 0;
	colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStateCreateInfo.attachmentCount = 1;
	colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
	float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	memcpy(colorBlendStateCreateInfo.blendConstants, blendConstants, sizeof(blendConstants));

	pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
}

void PipelineCI::ConfigDynamicStateCreateInfo()
{
	// Enable dynamic states
	// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
	// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
	// For this example we will set the viewport and scissor using dynamic states
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.pNext = nullptr;
	dynamicStateCreateInfo.flags = 0;
	dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicState.dynamicStates.size());
	dynamicStateCreateInfo.pDynamicStates = dynamicState.dynamicStates.data();

	pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
}

VulkanPipeline::VulkanPipeline(VulkanDevice * vulkanDevice, PipelineCI& pipelineCI) :
	m_VulkanDevice(vulkanDevice)
{
	VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_VulkanDevice->m_LogicalDevice, m_PipelineCache, 1, &pipelineCI.pipelineCreateInfo, nullptr, &m_Pipeline));
}

VulkanPipeline::~VulkanPipeline()
{
}

void VulkanPipeline::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_Pipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(m_VulkanDevice->m_LogicalDevice, m_Pipeline, nullptr);
		m_Pipeline = VK_NULL_HANDLE;
	}

	//vkDestroyPipelineCache
}
