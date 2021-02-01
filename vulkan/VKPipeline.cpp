#include "VKPipeline.h"
#include "VulkanTools.h"

PipelineCI::PipelineCI()
{
}

PipelineCI::~PipelineCI()
{
}

void PipelineCI::Reset(VkPipelineLayout layout, VkRenderPass renderPass, RenderStatus & renderStatus, VkShaderModule vertexSM, VkShaderModule framentSM)
{
	memset(this, 0, sizeof(*this));

	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = nullptr;
	pipelineCreateInfo.flags = 0;
	pipelineCreateInfo.stageCount = kVKShaderTypeCount;
	pipelineCreateInfo.pStages = shaderStageCreateInfos;
	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	pipelineCreateInfo.pTessellationState = &tessellationStateCreateInfo;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
	pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	pipelineCreateInfo.layout = layout;
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	{
		// Vertex shader
		shaderStageCreateInfos[kVKShaderTypeVertex].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfos[kVKShaderTypeVertex].pNext = nullptr;
		shaderStageCreateInfos[kVKShaderTypeVertex].flags = 0;
		shaderStageCreateInfos[kVKShaderTypeVertex].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStageCreateInfos[kVKShaderTypeVertex].module = vertexSM;
		shaderStageCreateInfos[kVKShaderTypeVertex].pName = "main";
		shaderStageCreateInfos[kVKShaderTypeVertex].pSpecializationInfo = nullptr;

		// Fragment shader
		shaderStageCreateInfos[kVKShaderTypeFragment].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfos[kVKShaderTypeFragment].pNext = nullptr;
		shaderStageCreateInfos[kVKShaderTypeFragment].flags = 0;
		shaderStageCreateInfos[kVKShaderTypeFragment].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStageCreateInfos[kVKShaderTypeFragment].module = framentSM;
		shaderStageCreateInfos[kVKShaderTypeFragment].pName = "main";
		shaderStageCreateInfos[kVKShaderTypeFragment].pSpecializationInfo = nullptr;
	}

	{
		// vertexInputStateCreateInfo需要具体mesh的顶点格式
	}

	{
		// Input assembly state describes how primitives are assembled
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.pNext = nullptr;
		inputAssemblyStateCreateInfo.flags = 0;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
	}

	{
		tessellationStateCreateInfo = {};
	}

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
	}

	{
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.pNext = nullptr;
		rasterizationStateCreateInfo.flags = 0;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationStateCreateInfo.lineWidth = 1.0f;
	}

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
	}

	{
		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.pNext = nullptr;
		depthStencilStateCreateInfo.flags = 0;

		depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;

		depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		depthStencilStateCreateInfo.front = {};
		depthStencilStateCreateInfo.back = {};

		depthStencilStateCreateInfo.minDepthBounds = 0.0f;
		depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
	}

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
	}

	{
		// Enable dynamic states
		// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
		// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
		// For this example we will set the viewport and scissor using dynamic states
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = nullptr;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(renderStatus.dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = renderStatus.dynamicStates.data();
	}
}

VKPipeline::VKPipeline(uint32_t currFrameIndex, VkDevice vkDevice, PipelineCI& pipelineCI, VertexDescription& vertexDescription) :
	VKResource(currFrameIndex)
{
	{
		// Inpute attribute bindings describe shader attribute locations and memory layouts
		uint32_t num = static_cast<uint32_t>(vertexDescription.formats.size());
		uint32_t stride = 0;
		for (uint32_t i = 0; i < num; i++)
		{
			pipelineCI.vertexInputAttributs[i].binding = 0;
			pipelineCI.vertexInputAttributs[i].location = i;
			pipelineCI.vertexInputAttributs[i].format = vertexDescription.formats[i];
			pipelineCI.vertexInputAttributs[i].offset = vertexDescription.offsets[i];
		}

		// Vertex input binding
		// This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
		pipelineCI.vertexInputBindings[0].binding = 0;
		pipelineCI.vertexInputBindings[0].stride = vertexDescription.stride;
		pipelineCI.vertexInputBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		// Vertex input state used for pipeline creation
		pipelineCI.vertexInputStateCreateInfo.pNext = nullptr;
		pipelineCI.vertexInputStateCreateInfo.flags = 0;
		pipelineCI.vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineCI.vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(1);
		pipelineCI.vertexInputStateCreateInfo.pVertexBindingDescriptions = pipelineCI.vertexInputBindings;
		pipelineCI.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = num;
		pipelineCI.vertexInputStateCreateInfo.pVertexAttributeDescriptions = pipelineCI.vertexInputAttributs;
	}
	
	VK_CHECK_RESULT(vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineCI.pipelineCreateInfo, nullptr, &pipeline));
}

VKPipeline::~VKPipeline()
{
}


