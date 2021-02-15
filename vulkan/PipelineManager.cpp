#include "PipelineManager.h"
#include "VulkanTools.h"
#include "GarbageCollector.h"
#include "VKGpuProgram.h"
#include "ProfilerManager.h"

PipelineCI::PipelineCI()
{
	memset(this, 0, sizeof(*this));
}

PipelineCI::~PipelineCI()
{
}

void PipelineCI::Reset(VKGpuProgram* vkGpuProgram, RenderState& renderState, void* scdata, VkRenderPass renderPass)
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
	pipelineCreateInfo.layout = vkGpuProgram->GetPipelineLayout();
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
		shaderStageCreateInfos[kVKShaderTypeVertex].module = vkGpuProgram->GetVertShaderModule();
		shaderStageCreateInfos[kVKShaderTypeVertex].pName = "main";
		shaderStageCreateInfos[kVKShaderTypeVertex].pSpecializationInfo = nullptr;

		// Fragment shader
		shaderStageCreateInfos[kVKShaderTypeFragment].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfos[kVKShaderTypeFragment].pNext = nullptr;
		shaderStageCreateInfos[kVKShaderTypeFragment].flags = 0;
		shaderStageCreateInfos[kVKShaderTypeFragment].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStageCreateInfos[kVKShaderTypeFragment].module = vkGpuProgram->GetFragShaderModule();
		shaderStageCreateInfos[kVKShaderTypeFragment].pName = "main";
		shaderStageCreateInfos[kVKShaderTypeFragment].pSpecializationInfo = nullptr;

		// SpecializationConstant
		// 测试发现很耗。。。不知道为啥
		if (vkGpuProgram->GetGpuParameters().SCParameters.size() > 0)
		{
			VkSpecializationInfo& si = vkGpuProgram->GetSpecializationInfo();
			si.pData = scdata;
			shaderStageCreateInfos[kVKShaderTypeFragment].pSpecializationInfo = &si;
		}
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
		rasterizationStateCreateInfo.cullMode = renderState.rasterizationState.cullMode;
		rasterizationStateCreateInfo.frontFace = renderState.rasterizationState.frontFace;
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

		depthStencilStateCreateInfo.depthTestEnable = renderState.depthStencilState.depthTestEnable;
		depthStencilStateCreateInfo.depthWriteEnable = renderState.depthStencilState.depthWriteEnable;
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
		colorBlendAttachmentState.blendEnable = renderState.blendState.blendEnable;
		colorBlendAttachmentState.srcColorBlendFactor = renderState.blendState.srcColorBlendFactor;
		colorBlendAttachmentState.dstColorBlendFactor = renderState.blendState.dstColorBlendFactor;
		colorBlendAttachmentState.colorBlendOp = renderState.blendState.colorBlendOp;
		colorBlendAttachmentState.srcAlphaBlendFactor = renderState.blendState.srcAlphaBlendFactor;
		colorBlendAttachmentState.dstAlphaBlendFactor = renderState.blendState.dstAlphaBlendFactor;
		colorBlendAttachmentState.alphaBlendOp = renderState.blendState.alphaBlendOp;
		colorBlendAttachmentState.colorWriteMask = renderState.blendState.colorWriteMask;

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
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(renderState.dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = renderState.dynamicStates.data();
	}
}

PipelineManager::PipelineManager(VkDevice vkDevice, GarbageCollector* gc) :
	m_Device(vkDevice),
	m_GarbageCollector(gc)
{
	m_PipelineCI = new PipelineCI();
}

PipelineManager::~PipelineManager()
{
	RELEASE(m_PipelineCI);
}

void PipelineManager::SetPipelineCI(VKGpuProgram * vkGpuProgram, RenderState & renderState, void* scdata, VkRenderPass renderPass)
{
	m_PipelineCI->Reset(vkGpuProgram, renderState, scdata, renderPass);
}

VkPipeline PipelineManager::CreatePipeline(VertexDescription & vertexDescription)
{
	PROFILER(CreatePipeline);

	VKPipeline* pipeline = new VKPipeline(m_Device);

	pipeline->Use(m_GarbageCollector->GetFrameIndex());
	m_GarbageCollector->AddResource(pipeline);

	PipelineCI& pipelineCI = *m_PipelineCI;
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

	PROFILER_BEGIN(CreateGraphicsPipelines);
	VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineCI.pipelineCreateInfo, nullptr, &pipeline->pipeline));
	PROFILER_END(CreateGraphicsPipelines);

	return pipeline->pipeline;
}
