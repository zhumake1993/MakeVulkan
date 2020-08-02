#include "VulkanBase.h"
#include "Tools.h"

VulkanBase::VulkanBase()
{
}

VulkanBase::~VulkanBase()
{
}

void VulkanBase::CleanUp()
{
	m_VulkanDevice->WaitIdle();


	m_VulkanCommandPool->CleanUp();


	m_VulkanSwapChain->CleanUp();
	m_VulkanDevice->CleanUp();
	m_VulkanSurface->CleanUp();
	m_VulkanInstance->CleanUp();
}

void VulkanBase::Init()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	if (!InitVulkan())
	{
		LOG("Vulkan is unavailable, install vulkan and re-start");
		assert(false);
	}
	LOG("Vulkan Ready");
#endif

	m_VulkanInstance = new VulkanInstance();

#if defined(_WIN32)
	m_VulkanSurface = new VulkanSurface(m_VulkanInstance, m_WindowInstance, m_WindowHandle);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_VulkanSurface = new VulkanSurface(m_VulkanInstance, androidApp->window);
#endif

	m_VulkanDevice = new VulkanDevice(m_VulkanInstance, m_VulkanSurface);
	m_VulkanSwapChain = new VulkanSwapChain(m_VulkanDevice, m_VulkanSurface);



	m_VulkanCommandPool = m_VulkanDevice.CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_VulkanDevice.m_SelectedQueueFamilyIndex);

	m_FrameResources.resize(global::frameResourcesCount);
	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_FrameResources[i].vulkanCommandBuffer = m_VulkanDevice.AllocateCommandBuffers(m_VulkanCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		m_FrameResources[i].imageAvailableSemaphore = m_VulkanDevice.CreateSemaphore();
		m_FrameResources[i].finishedRenderingSemaphore = m_VulkanDevice.CreateSemaphore();
		m_FrameResources[i].fence = m_VulkanDevice.CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
	}

	SetupRenderPass();
	CreatePipeline();
	PrepareVertices();

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	// Get Android device name and manufacturer (to display along GPU name)
	androidProduct = "";
	char prop[PROP_VALUE_MAX + 1];
	int len = __system_property_get("ro.product.manufacturer", prop);
	if (len > 0) {
		androidProduct += std::string(prop) + " ";
	};
	len = __system_property_get("ro.product.model", prop);
	if (len > 0) {
		androidProduct += std::string(prop);
	};
	LOG("androidProduct = %s", androidProduct.c_str());
#endif

}

void VulkanBase::Prepare()
{
	m_CanRender = true;
}

void VulkanBase::SetupRenderPass()
{
	// This example will use a single render pass with one subpass

	// Descriptors for the attachments used by this renderpass
	std::vector<VkAttachmentDescription> attachmentDescriptions(1);

	// Color attachment
	attachmentDescriptions[0].flags = 0;
	attachmentDescriptions[0].format = m_VulkanSwapChain.m_Format.format;                                  // Use the color format selected by the swapchain
	attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;                                 // We don't use multi sampling in this example
	attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear this attachment at the start of the render pass
	attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                          // Keep its contents after the render pass is finished (for displaying it)
	attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // We don't use stencil, so don't care for load
	attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // Same for store
	attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
	attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;                   // Layout to which the attachment is transitioned when the render pass is finished
																					// As we want to present the color buffer to the swapchain, we transition to PRESENT_KHR

	// Setup attachment references
	VkAttachmentReference colorReference = {};
	colorReference.attachment = 0;                                    // Attachment 0 is color
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Attachment layout used as color during the subpass

	// Setup a single subpass reference
	VkSubpassDescription subpassDescription = {};
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;                            // Input attachments can be used to sample from contents of a previous subpass
	subpassDescription.pInputAttachments = nullptr;                         // (Input attachments not used by this example)
	subpassDescription.colorAttachmentCount = 1;                            // Subpass uses one color attachment
	subpassDescription.pColorAttachments = &colorReference;                 // Reference to the color attachment in slot 0
	subpassDescription.pResolveAttachments = nullptr;                       // Resolve attachments are resolved at the end of a sub pass and can be used for e.g. multi sampling
	subpassDescription.pDepthStencilAttachment = nullptr;           // Reference to the depth attachment in slot 1
	subpassDescription.preserveAttachmentCount = 0;                         // Preserved attachments can be used to loop (and preserve) attachments through subpasses
	subpassDescription.pPreserveAttachments = nullptr;                      // (Preserve attachments not used by this example)

	// Setup subpass dependencies
	// These will add the implicit ttachment layout transitionss specified by the attachment descriptions
	// The actual usage layout is preserved through the layout specified in the attachment reference
	// Each subpass dependency will introduce a memory and execution dependency between the source and dest subpass described by
	// srcStageMask, dstStageMask, srcAccessMask, dstAccessMask (and dependencyFlags is set)
	// Note: VK_SUBPASS_EXTERNAL is a special constant that refers to all commands executed outside of the actual renderpass)
	std::vector<VkSubpassDependency> dependencies(2);

	// First dependency at the start of the renderpass
	// Does the transition from final to initial layout
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;                             // Producer of the dependency
	dependencies[0].dstSubpass = 0;                                               // Consumer is our single subpass that will wait for the execution depdendency
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Match our pWaitDstStageMask when we vkQueueSubmit
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // is a loadOp stage for color attachments
	dependencies[0].srcAccessMask = 0;                                            // semaphore wait already does memory dependency for us
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;         // is a loadOp CLEAR access mask for color attachments
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Second dependency at the end the renderpass
	// Does the transition from the initial to the final layout
	// Technically this is the same as the implicit subpass dependency, but we are gonna state it explicitly here
	dependencies[1].srcSubpass = 0;                                               // Producer of the dependency is our single subpass
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;                             // Consumer are all commands outside of the renderpass
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // is a storeOp stage for color attachments
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;          // Do not block any subsequent work
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;         // is a storeOp `STORE` access mask for color attachments
	dependencies[1].dstAccessMask = 0;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create the actual renderpass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.flags = 0;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());  // Number of attachments used by this render pass
	renderPassInfo.pAttachments = attachmentDescriptions.data();                            // Descriptions of the attachments used by the render pass
	renderPassInfo.subpassCount = 1;                                             // We only use one subpass in this example
	renderPassInfo.pSubpasses = &subpassDescription;                             // Description of that subpass
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size()); // Number of subpass dependencies
	renderPassInfo.pDependencies = dependencies.data();                          // Subpass dependencies used by the render pass

	VK_CHECK_RESULT(vkCreateRenderPass(*m_VulkanDevice, &renderPassInfo, nullptr, &m_RenderPass));
}

void VulkanBase::CreatePipeline()
{
	VulkanShaderModule vulkanShaderModuleVert = m_VulkanDevice.CreateShaderModule(GetAssetPath() + "shaders/shader.vert.spv");
	VulkanShaderModule vulkanShaderModuleFrag = m_VulkanDevice.CreateShaderModule(GetAssetPath() + "shaders/shader.frag.spv");

	// Shaders
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos(2);

	// Vertex shader
	shaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfos[0].pNext = nullptr;
	shaderStageCreateInfos[0].flags = 0;
	shaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageCreateInfos[0].module = vulkanShaderModuleVert.m_ShaderModule;
	shaderStageCreateInfos[0].pName = "main";
	shaderStageCreateInfos[0].pSpecializationInfo = nullptr;

	// Fragment shader
	shaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfos[1].pNext = nullptr;
	shaderStageCreateInfos[1].flags = 0;
	shaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStageCreateInfos[1].module = vulkanShaderModuleFrag.m_ShaderModule;
	shaderStageCreateInfos[1].pName = "main";
	shaderStageCreateInfos[1].pSpecializationInfo = nullptr;

	// Vertex input binding
	// This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
	std::vector<VkVertexInputBindingDescription> vertexInputBindings(1);
	vertexInputBindings[0].binding = 0;
	vertexInputBindings[0].stride = sizeof(VertexData);
	vertexInputBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// Inpute attribute bindings describe shader attribute locations and memory layouts
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributs(2);
	vertexInputAttributs[0].binding = 0;
	vertexInputAttributs[0].location = 0;
	vertexInputAttributs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexInputAttributs[0].offset = offsetof(VertexData, x);
	vertexInputAttributs[1].binding = 0;
	vertexInputAttributs[1].location = 1;
	vertexInputAttributs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexInputAttributs[1].offset = offsetof(VertexData, r);

	// Vertex input state used for pipeline creation
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
	vertexInputStateCreateInfo.pNext = nullptr;
	vertexInputStateCreateInfo.flags = 0;
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
	vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindings.data();
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributs.size());
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributs.data();


	// Input assembly state describes how primitives are assembled
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.pNext = nullptr;
	inputAssemblyStateCreateInfo.flags = 0;
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	// Viewport state sets the number of viewports and scissor used in this pipeline
	// Note: This is actually overriden by the dynamic states (see below)
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.pNext = nullptr;
	viewportStateCreateInfo.flags = 0;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = nullptr;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = nullptr;

	// Enable dynamic states
	// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
	// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
	// For this example we will set the viewport and scissor using dynamic states
	std::vector<VkDynamicState> dynamicStates;
	dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.pNext = nullptr;
	dynamicStateCreateInfo.flags = 0;
	dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
	
	// Rasterization state
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
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

	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateCreateInfo.pNext = nullptr;
	multisampleStateCreateInfo.flags = 0;
	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.minSampleShading = 1.0f;
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	// Color blend state describes how blend factors are calculated (if used)
	// We need one blend attachment state per color attachment (even if blending is not used)
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateCreateInfo.pNext = nullptr;
	colorBlendStateCreateInfo.flags = 0;
	colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStateCreateInfo.attachmentCount = 1;
	colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
	float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	memcpy(colorBlendStateCreateInfo.blendConstants, blendConstants, sizeof(blendConstants));

	// Create the pipeline layout that is used to generate the rendering pipelines that are based on this descriptor set layout
	// In a more complex scenario you would have different pipeline layouts for different descriptor set layouts that could be reused
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	VK_CHECK_RESULT(vkCreatePipelineLayout(*m_VulkanDevice, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = nullptr;
	pipelineCreateInfo.flags = 0;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
	pipelineCreateInfo.pStages = shaderStageCreateInfos.data();
	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	pipelineCreateInfo.pTessellationState = nullptr;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = nullptr;
	pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
	pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	pipelineCreateInfo.layout = m_PipelineLayout;
	pipelineCreateInfo.renderPass = m_RenderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(*m_VulkanDevice, m_PipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline));
}

void VulkanBase::PrepareVertices()
{
	std::vector<VertexData> vertexBuffer =
	{
		{   1.0f,  1.0f, 0.0f,  1.0f ,  1.0f, 0.0f, 0.0f,  0.0f  },
		{  -1.0f,  1.0f, 0.0f,  1.0f ,  0.0f, 1.0f, 0.0f,  0.0f  },
		{   0.0f, -1.0f, 0.0f,  1.0f ,  0.0f, 0.0f, 1.0f,  0.0f  }
	};
	uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(VertexData);

	// Don't use staging
	// Create host-visible buffers only and use these for rendering. This is not advised and will usually result in lower rendering performance

	// Vertex buffer
	VkBufferCreateInfo vertexBufferInfo = {};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = nullptr;
	vertexBufferInfo.flags = 0;
	vertexBufferInfo.size = vertexBufferSize;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vertexBufferInfo.queueFamilyIndexCount = 0;
	vertexBufferInfo.pQueueFamilyIndices = nullptr;

	// Copy vertex data to a buffer visible to the host
	VK_CHECK_RESULT(vkCreateBuffer(*m_VulkanDevice, &vertexBufferInfo, nullptr, &m_VertexBuffer));

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(*m_VulkanDevice, m_VertexBuffer, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is host visible memory, and VK_MEMORY_PROPERTY_HOST_COHERENT_BIT makes sure writes are directly visible
	memoryAllocateInfo.memoryTypeIndex = m_VulkanDevice.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(*m_VulkanDevice, &memoryAllocateInfo, nullptr, &m_VertexBufferMemory));

	VK_CHECK_RESULT(vkBindBufferMemory(*m_VulkanDevice, m_VertexBuffer, m_VertexBufferMemory, 0));

	void *vertexBufferMemoryPointer;
	VK_CHECK_RESULT(vkMapMemory(*m_VulkanDevice, m_VertexBufferMemory, 0, vertexBufferSize, 0, &vertexBufferMemoryPointer));
	memcpy(vertexBufferMemoryPointer, vertexBuffer.data(), vertexBufferSize);

	// 由于使用了VK_MEMORY_PROPERTY_HOST_COHERENT_BIT，因此不需要flush
	vkUnmapMemory(*m_VulkanDevice, m_VertexBufferMemory);


	// Index buffer
	std::vector<uint32_t> indexBuffer = { 0, 1, 2 };
	uint32_t indexBufferSize = static_cast<uint32_t>(indexBuffer.size()) * sizeof(uint32_t);

	VkBufferCreateInfo indexBufferInfo = {};
	indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	indexBufferInfo.pNext = nullptr;
	indexBufferInfo.flags = 0;
	indexBufferInfo.size = indexBufferSize;
	indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	indexBufferInfo.queueFamilyIndexCount = 0;
	indexBufferInfo.pQueueFamilyIndices = nullptr;

	VK_CHECK_RESULT(vkCreateBuffer(*m_VulkanDevice, &indexBufferInfo, nullptr, &m_IndexBuffer));

	//VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(*m_VulkanDevice, m_IndexBuffer, &memReqs);

	//VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is host visible memory, and VK_MEMORY_PROPERTY_HOST_COHERENT_BIT makes sure writes are directly visible
	memoryAllocateInfo.memoryTypeIndex = m_VulkanDevice.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(*m_VulkanDevice, &memoryAllocateInfo, nullptr, &m_IndexBufferMemory));

	VK_CHECK_RESULT(vkBindBufferMemory(*m_VulkanDevice, m_IndexBuffer, m_IndexBufferMemory, 0));

	void *indexBufferMemoryPointer;
	VK_CHECK_RESULT(vkMapMemory(*m_VulkanDevice, m_IndexBufferMemory, 0, indexBufferSize, 0, &indexBufferMemoryPointer));
	memcpy(indexBufferMemoryPointer, indexBuffer.data(), indexBufferSize);

	// 由于使用了VK_MEMORY_PROPERTY_HOST_COHERENT_BIT，因此不需要flush
	vkUnmapMemory(*m_VulkanDevice, m_IndexBufferMemory);
}

void VulkanBase::Draw()
{
	auto& currFrameResource = m_FrameResources[m_CurrFrameIndex];
	m_CurrFrameIndex = (m_CurrFrameIndex + 1) % global::frameResourcesCount;

	VK_CHECK_RESULT(vkWaitForFences(*m_VulkanDevice, 1, &currFrameResource.fence, VK_TRUE, UINT64_MAX));
	VK_CHECK_RESULT(vkResetFences(*m_VulkanDevice, 1, &currFrameResource.fence));

	uint32_t imageIndex = m_VulkanSwapChain.AcquireNextImage(currFrameResource.imageAvailableSemaphore);

	CreateFramebuffer(currFrameResource.framebuffer, m_VulkanSwapChain.m_SwapChainImageViews[imageIndex]);

	RecordCommandBuffer(currFrameResource.vulkanCommandBuffer, currFrameResource.framebuffer);

	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &currFrameResource.imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &currFrameResource.vulkanCommandBuffer.m_CommandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &currFrameResource.finishedRenderingSemaphore;

	VK_CHECK_RESULT(vkQueueSubmit(m_VulkanDevice.m_Queue, 1, &submitInfo, currFrameResource.fence));

	m_VulkanSwapChain.QueuePresent(imageIndex, currFrameResource.finishedRenderingSemaphore);
}

void VulkanBase::RecordCommandBuffer(VulkanCommandBuffer& vulkanCommandBuffer, VkFramebuffer& framebuffer)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
	VK_CHECK_RESULT(vkBeginCommandBuffer(*vulkanCommandBuffer, &commandBufferBeginInfo));

	// 暂时没有depth
	VkClearValue clearValues[1];
	clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = m_RenderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = m_VulkanSwapChain.m_Extent;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;
	vkCmdBeginRenderPass(*vulkanCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_VulkanSwapChain.m_Extent.width);
	viewport.height = static_cast<float>(m_VulkanSwapChain.m_Extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(*vulkanCommandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = m_VulkanSwapChain.m_Extent;
	vkCmdSetScissor(*vulkanCommandBuffer, 0, 1, &scissor);

	vkCmdBindPipeline(*vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(*vulkanCommandBuffer, 0, 1, &m_VertexBuffer, offsets);

	// Bind triangle index buffer
	vkCmdBindIndexBuffer(*vulkanCommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	// Draw indexed triangle
	vkCmdDrawIndexed(*vulkanCommandBuffer, 3, 1, 0, 0, 1);

	vkCmdEndRenderPass(*vulkanCommandBuffer);

	VK_CHECK_RESULT(vkEndCommandBuffer(*vulkanCommandBuffer));
}

void VulkanBase::CreateFramebuffer(VkFramebuffer& framebuffer, VkImageView& imageView)
{
	if (framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(*m_VulkanDevice, framebuffer, nullptr);
		framebuffer = VK_NULL_HANDLE;
	}

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.flags = 0;
	frameBufferCreateInfo.renderPass = m_RenderPass;
	frameBufferCreateInfo.attachmentCount = 1;
	frameBufferCreateInfo.pAttachments = &imageView;
	frameBufferCreateInfo.width = m_VulkanSwapChain.m_Extent.width;
	frameBufferCreateInfo.height = m_VulkanSwapChain.m_Extent.height;
	frameBufferCreateInfo.layers = 1;
	VK_CHECK_RESULT(vkCreateFramebuffer(*m_VulkanDevice, &frameBufferCreateInfo, nullptr, &framebuffer));
}
