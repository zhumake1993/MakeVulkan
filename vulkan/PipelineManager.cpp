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

void PipelineCI::Reset(VKGpuProgram* vkGpuProgram, RenderState* renderState, void* scdata, VkRenderPass renderPass, uint32_t subPassIndex)
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
	pipelineCreateInfo.subpass = subPassIndex;
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
		rasterizationStateCreateInfo.cullMode = renderState->rasterizationState.cullMode;
		rasterizationStateCreateInfo.frontFace = renderState->rasterizationState.frontFace;
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

		depthStencilStateCreateInfo.depthTestEnable = renderState->depthStencilState.depthTestEnable;
		depthStencilStateCreateInfo.depthWriteEnable = renderState->depthStencilState.depthWriteEnable;
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
		colorBlendAttachmentStates.resize(renderState->blendStates.size());
		for (size_t i = 0; i < renderState->blendStates.size(); i++)
		{
			colorBlendAttachmentStates[i].blendEnable = renderState->blendStates[i].blendEnable;
			colorBlendAttachmentStates[i].srcColorBlendFactor = renderState->blendStates[i].srcColorBlendFactor;
			colorBlendAttachmentStates[i].dstColorBlendFactor = renderState->blendStates[i].dstColorBlendFactor;
			colorBlendAttachmentStates[i].colorBlendOp = renderState->blendStates[i].colorBlendOp;
			colorBlendAttachmentStates[i].srcAlphaBlendFactor = renderState->blendStates[i].srcAlphaBlendFactor;
			colorBlendAttachmentStates[i].dstAlphaBlendFactor = renderState->blendStates[i].dstAlphaBlendFactor;
			colorBlendAttachmentStates[i].alphaBlendOp = renderState->blendStates[i].alphaBlendOp;
			colorBlendAttachmentStates[i].colorWriteMask = renderState->blendStates[i].colorWriteMask;
		}

		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = nullptr;
		colorBlendStateCreateInfo.flags = 0;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
		colorBlendStateCreateInfo.pAttachments = colorBlendAttachmentStates.data();
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
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(renderState->dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = renderState->dynamicStates.data();
	}
}

PipelineManager::PipelineManager(VkDevice vkDevice) :
	m_Device(vkDevice)
{
	VkPipelineCacheCreateInfo cacheCI = {};
	cacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	cacheCI.pNext = nullptr;
	cacheCI.flags = 0;
	cacheCI.initialDataSize = 0;
	cacheCI.pInitialData = nullptr;

	VK_CHECK_RESULT(vkCreatePipelineCache(m_Device, &cacheCI, nullptr, &m_PipelineCache));
}

PipelineManager::~PipelineManager()
{
	for (auto& p : m_PSOCache)
	{
		vkDestroyPipeline(m_Device, p.second->pipeline, nullptr);
		RELEASE(p.second);
	}

	vkDestroyPipelineCache(m_Device, m_PipelineCache, nullptr);
}

void PipelineManager::Update()
{
	m_FrameIndex++;
}

void PipelineManager::SetPipelineKey(VKGpuProgram * vkGpuProgram, RenderState * renderState, void * scdata, VkRenderPass renderPass, uint32_t subPassIndex)
{
	m_PipelineKey.vkGpuProgram = vkGpuProgram;
	m_PipelineKey.renderState = renderState;
	m_PipelineKey.scdata = scdata;
	m_PipelineKey.renderPass = renderPass;
	m_PipelineKey.subPassIndex = subPassIndex;
}

VkPipeline PipelineManager::CreatePipeline(VertexDescription * vertexDescription)
{
	m_PipelineKey.vertexDescription = vertexDescription;

	if (m_PSOCache.find(m_PipelineKey) == m_PSOCache.end())
	{
		Pipeline* pipeline = CreatePipelineInternal(m_PipelineKey);
		m_PSOCache[m_PipelineKey] = pipeline;
		return pipeline->pipeline;
	}
	else
	{
		return m_PSOCache[m_PipelineKey]->pipeline;
	}
}

PipelineManager::Pipeline* PipelineManager::CreatePipelineInternal(PipelineKey & pipelineKey)
{
	Pipeline* pipeline = new Pipeline();
	pipeline->Use();

	PipelineCI pipelineCI;

	pipelineCI.Reset(pipelineKey.vkGpuProgram, pipelineKey.renderState, pipelineKey.scdata, pipelineKey.renderPass, pipelineKey.subPassIndex);

	if(pipelineKey.vertexDescription)
	{
		auto& vertexDescription = *pipelineKey.vertexDescription;

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
	else
	{
		pipelineCI.vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	}

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_Device, m_PipelineCache, 1, &pipelineCI.pipelineCreateInfo, nullptr, &pipeline->pipeline));

	return pipeline;
}
