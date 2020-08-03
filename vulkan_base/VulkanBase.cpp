#include "VulkanBase.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "VulkanFrameBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
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

	m_VulkanPipeline->CleanUp();

	m_VulkanRenderPass->CleanUp();

	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_FrameResources[i].framebuffer->CleanUp();
		m_FrameResources[i].commandBuffer->CleanUp();
		m_FrameResources[i].imageAvailableSemaphore->CleanUp();
		m_FrameResources[i].finishedRenderingSemaphore->CleanUp();
		m_FrameResources[i].fence->CleanUp();
	}

	m_VulkanCommandPool->CleanUp();
	m_VulkanSwapChain->CleanUp();
	m_VulkanDevice->CleanUp();
	m_VulkanSurface->CleanUp();
	m_VulkanInstance->CleanUp();
}

void VulkanBase::Init()
{
	m_VulkanInstance = new VulkanInstance();

#if defined(_WIN32)
	m_VulkanSurface = new VulkanSurface(m_VulkanInstance, m_WindowInstance, m_WindowHandle);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_VulkanSurface = new VulkanSurface(m_VulkanInstance, androidApp->window);
#endif

	m_VulkanDevice = new VulkanDevice(m_VulkanInstance, m_VulkanSurface);
	m_VulkanSwapChain = new VulkanSwapChain(m_VulkanDevice, m_VulkanSurface);
	m_VulkanCommandPool = new VulkanCommandPool(m_VulkanDevice, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_VulkanDevice->m_SelectedQueueFamilyIndex);


	m_FrameResources.resize(global::frameResourcesCount);
	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_FrameResources[i].framebuffer = new VulkanFramebuffer(m_VulkanDevice);
		m_FrameResources[i].commandBuffer = m_VulkanCommandPool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		m_FrameResources[i].imageAvailableSemaphore = new VulkanSemaphore(m_VulkanDevice);
		m_FrameResources[i].finishedRenderingSemaphore = new VulkanSemaphore(m_VulkanDevice);
		m_FrameResources[i].fence = new VulkanFence(m_VulkanDevice, true);
	}

	m_VulkanRenderPass = new VulkanRenderPass(m_VulkanDevice, m_VulkanSwapChain->m_Format.format);

	// std::make_unique 需要C++14的支持，这里使用构造函数更保险
	std::shared_ptr<VulkanShaderModule> vulkanShaderModuleVert = std::shared_ptr<VulkanShaderModule>(new VulkanShaderModule(m_VulkanDevice, GetAssetPath() + "shaders/shader.vert.spv"));
	std::shared_ptr<VulkanShaderModule> vulkanShaderModuleFrag = std::shared_ptr<VulkanShaderModule>(new VulkanShaderModule(m_VulkanDevice, GetAssetPath() + "shaders/shader.frag.spv"));
	std::shared_ptr<VulkanPipelineLayout> vulkanPipelineLayout = std::shared_ptr<VulkanPipelineLayout>(new VulkanPipelineLayout(m_VulkanDevice));

	PipelineStatus pipelineStatus;

	pipelineStatus.shaderStage.vertShaderModule = vulkanShaderModuleVert;
	pipelineStatus.shaderStage.fragShaderModule = vulkanShaderModuleFrag;

	pipelineStatus.vertexInputState.vertexLayout.push_back(kVertexFormatFloat32x4);
	pipelineStatus.vertexInputState.vertexLayout.push_back(kVertexFormatFloat32x4);

	pipelineStatus.dynamicState.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	pipelineStatus.dynamicState.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

	m_VulkanPipeline = new VulkanPipeline(m_VulkanDevice, &pipelineStatus, vulkanPipelineLayout, m_VulkanRenderPass);

	PrepareVertices();
}

void VulkanBase::Prepare()
{
	m_CanRender = true;
}

void VulkanBase::CreatePipeline(std::shared_ptr<VulkanPipelineLayout> vulkanPipelineLayout)
{
	//
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
	VK_CHECK_RESULT(vkCreateBuffer(m_VulkanDevice->m_LogicalDevice, &vertexBufferInfo, nullptr, &m_VertexBuffer));

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(m_VulkanDevice->m_LogicalDevice, m_VertexBuffer, &memReqs);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is host visible memory, and VK_MEMORY_PROPERTY_HOST_COHERENT_BIT makes sure writes are directly visible
	memoryAllocateInfo.memoryTypeIndex = m_VulkanDevice->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_VulkanDevice->m_LogicalDevice, &memoryAllocateInfo, nullptr, &m_VertexBufferMemory));

	VK_CHECK_RESULT(vkBindBufferMemory(m_VulkanDevice->m_LogicalDevice, m_VertexBuffer, m_VertexBufferMemory, 0));

	void *vertexBufferMemoryPointer;
	VK_CHECK_RESULT(vkMapMemory(m_VulkanDevice->m_LogicalDevice, m_VertexBufferMemory, 0, vertexBufferSize, 0, &vertexBufferMemoryPointer));
	memcpy(vertexBufferMemoryPointer, vertexBuffer.data(), vertexBufferSize);

	// 由于使用了VK_MEMORY_PROPERTY_HOST_COHERENT_BIT，因此不需要flush
	vkUnmapMemory(m_VulkanDevice->m_LogicalDevice, m_VertexBufferMemory);


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

	VK_CHECK_RESULT(vkCreateBuffer(m_VulkanDevice->m_LogicalDevice, &indexBufferInfo, nullptr, &m_IndexBuffer));

	//VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(m_VulkanDevice->m_LogicalDevice, m_IndexBuffer, &memReqs);

	//VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memReqs.size;
	// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is host visible memory, and VK_MEMORY_PROPERTY_HOST_COHERENT_BIT makes sure writes are directly visible
	memoryAllocateInfo.memoryTypeIndex = m_VulkanDevice->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(m_VulkanDevice->m_LogicalDevice, &memoryAllocateInfo, nullptr, &m_IndexBufferMemory));

	VK_CHECK_RESULT(vkBindBufferMemory(m_VulkanDevice->m_LogicalDevice, m_IndexBuffer, m_IndexBufferMemory, 0));

	void *indexBufferMemoryPointer;
	VK_CHECK_RESULT(vkMapMemory(m_VulkanDevice->m_LogicalDevice, m_IndexBufferMemory, 0, indexBufferSize, 0, &indexBufferMemoryPointer));
	memcpy(indexBufferMemoryPointer, indexBuffer.data(), indexBufferSize);

	// 由于使用了VK_MEMORY_PROPERTY_HOST_COHERENT_BIT，因此不需要flush
	vkUnmapMemory(m_VulkanDevice->m_LogicalDevice, m_IndexBufferMemory);
}

void VulkanBase::Draw()
{
	auto& currFrameResource = m_FrameResources[m_CurrFrameIndex];
	m_CurrFrameIndex = (m_CurrFrameIndex + 1) % global::frameResourcesCount;

	currFrameResource.fence->Wait();
	currFrameResource.fence->Reset();

	uint32_t imageIndex = m_VulkanSwapChain->AcquireNextImage(currFrameResource.imageAvailableSemaphore);

	CreateFramebuffer(currFrameResource.framebuffer, m_VulkanSwapChain->m_SwapChainImageViews[imageIndex]);

	RecordCommandBuffer(currFrameResource.commandBuffer, currFrameResource.framebuffer);

	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &currFrameResource.imageAvailableSemaphore->m_Semaphore;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &currFrameResource.commandBuffer->m_CommandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &currFrameResource.finishedRenderingSemaphore->m_Semaphore;

	VK_CHECK_RESULT(vkQueueSubmit(m_VulkanDevice->m_Queue, 1, &submitInfo, currFrameResource.fence->m_Fence));

	m_VulkanSwapChain->QueuePresent(imageIndex, currFrameResource.finishedRenderingSemaphore);
}

void VulkanBase::RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer, VulkanFramebuffer* vulkanFramebuffer)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
	VK_CHECK_RESULT(vkBeginCommandBuffer(vulkanCommandBuffer->m_CommandBuffer, &commandBufferBeginInfo));

	// 暂时没有depth
	VkClearValue clearValues[1];
	clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = m_VulkanRenderPass->m_RenderPass;
	renderPassBeginInfo.framebuffer = vulkanFramebuffer->m_Framebuffer;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = m_VulkanSwapChain->m_Extent;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = clearValues;
	vkCmdBeginRenderPass(vulkanCommandBuffer->m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_VulkanSwapChain->m_Extent.width);
	viewport.height = static_cast<float>(m_VulkanSwapChain->m_Extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vulkanCommandBuffer->m_CommandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = m_VulkanSwapChain->m_Extent;
	vkCmdSetScissor(vulkanCommandBuffer->m_CommandBuffer, 0, 1, &scissor);

	vkCmdBindPipeline(vulkanCommandBuffer->m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline->m_Pipeline);

	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(vulkanCommandBuffer->m_CommandBuffer, 0, 1, &m_VertexBuffer, offsets);

	// Bind triangle index buffer
	vkCmdBindIndexBuffer(vulkanCommandBuffer->m_CommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	// Draw indexed triangle
	vkCmdDrawIndexed(vulkanCommandBuffer->m_CommandBuffer, 3, 1, 0, 0, 1);

	vkCmdEndRenderPass(vulkanCommandBuffer->m_CommandBuffer);

	VK_CHECK_RESULT(vkEndCommandBuffer(vulkanCommandBuffer->m_CommandBuffer));
}

void VulkanBase::CreateFramebuffer(VulkanFramebuffer* vulkanFramebuffer, VkImageView& imageView)
{
	if (vulkanFramebuffer->m_Framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(m_VulkanDevice->m_LogicalDevice, vulkanFramebuffer->m_Framebuffer, nullptr);
		vulkanFramebuffer->m_Framebuffer = VK_NULL_HANDLE;
	}

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.flags = 0;
	frameBufferCreateInfo.renderPass = m_VulkanRenderPass->m_RenderPass;
	frameBufferCreateInfo.attachmentCount = 1;
	frameBufferCreateInfo.pAttachments = &imageView;
	frameBufferCreateInfo.width = m_VulkanSwapChain->m_Extent.width;
	frameBufferCreateInfo.height = m_VulkanSwapChain->m_Extent.height;
	frameBufferCreateInfo.layers = 1;
	VK_CHECK_RESULT(vkCreateFramebuffer(m_VulkanDevice->m_LogicalDevice, &frameBufferCreateInfo, nullptr, &vulkanFramebuffer->m_Framebuffer));
}
