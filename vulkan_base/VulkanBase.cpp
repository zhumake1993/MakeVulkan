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
#include "VulkanBuffer.h"
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

	m_VertexBuffer->CleanUp();
	m_IndexBuffer->CleanUp();
	m_VertexStagingBuffer->CleanUp();
	m_IndexStagingBuffer->CleanUp();

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
	// Vertex buffer
	std::vector<float> vertexBuffer =
	{
		   1.0f,  1.0f, 0.0f,  1.0f ,  1.0f, 0.0f, 0.0f,  0.0f  ,
		  -1.0f,  1.0f, 0.0f,  1.0f ,  0.0f, 1.0f, 0.0f,  0.0f  ,
		   0.0f, -1.0f, 0.0f,  1.0f ,  0.0f, 0.0f, 1.0f,  0.0f
	};
	uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(vertexBuffer[0]);

	m_VertexBuffer = new VulkanBuffer(m_VulkanDevice, vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_VertexStagingBuffer = new VulkanBuffer(m_VulkanDevice, 1024, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	void* vertexBufferMemoryPointer = m_VertexStagingBuffer->Map(vertexBufferSize);
	memcpy(vertexBufferMemoryPointer, vertexBuffer.data(), vertexBufferSize);
	m_VertexStagingBuffer->Unmap();

	// Index buffer
	std::vector<uint32_t> indexBuffer = { 0, 1, 2 };
	uint32_t indexBufferSize = static_cast<uint32_t>(indexBuffer.size()) * sizeof(uint32_t);

	m_IndexBuffer = new VulkanBuffer(m_VulkanDevice, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_IndexStagingBuffer = new VulkanBuffer(m_VulkanDevice, 1024, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	void* indexBufferMemoryPointer = m_IndexStagingBuffer->Map(indexBufferSize);
	memcpy(indexBufferMemoryPointer, indexBuffer.data(), indexBufferSize);
	m_IndexStagingBuffer->Unmap();

	// cmd
	auto cmd = m_VulkanCommandPool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	cmd->Begin();

	VkBufferCopy bufferCopyInfo = {};
	bufferCopyInfo.srcOffset = 0;
	bufferCopyInfo.dstOffset = 0;

	bufferCopyInfo.size = vertexBufferSize;
	cmd->CopyBuffer(m_VertexStagingBuffer, m_VertexBuffer, bufferCopyInfo);
	bufferCopyInfo.size = indexBufferSize;
	cmd->CopyBuffer(m_IndexStagingBuffer, m_IndexBuffer, bufferCopyInfo);

	// 经测试发现没有这一步也没问题（许多教程也的确没有这一步）
	/*VkBufferMemoryBarrier bufferMemoryBarrier = {};
	bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	bufferMemoryBarrier.pNext = nullptr;
	bufferMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
	bufferMemoryBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBarrier.buffer = m_VertexBuffer->m_Buffer;
	bufferMemoryBarrier.offset = 0;
	bufferMemoryBarrier.size = VK_WHOLE_SIZE;
	vkCmdPipelineBarrier(cmd->m_CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);*/

	cmd->End();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd->m_CommandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	VK_CHECK_RESULT(vkQueueSubmit(m_VulkanDevice->m_Queue, 1, &submitInfo, VK_NULL_HANDLE));

	m_VulkanDevice->WaitIdle();
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
	VkClearValue clearValue = {};
	clearValue.color = { 0.0f, 0.0f, 0.2f, 1.0f };
	clearValue.depthStencil = { 0.0f, 0 };

	VkRect2D area = {};
	area.offset.x = 0;
	area.offset.y = 0;
	area.extent = m_VulkanSwapChain->m_Extent;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_VulkanSwapChain->m_Extent.width);
	viewport.height = static_cast<float>(m_VulkanSwapChain->m_Extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vulkanCommandBuffer->Begin();

	vulkanCommandBuffer->BeginRenderPass(m_VulkanRenderPass, vulkanFramebuffer, area, clearValue);
	
	vulkanCommandBuffer->SetViewport(viewport);

	vulkanCommandBuffer->SetScissor(area);

	vulkanCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline);

	vulkanCommandBuffer->BindVertexBuffer(0, m_VertexBuffer);

	vulkanCommandBuffer->BindIndexBuffer(m_IndexBuffer, VK_INDEX_TYPE_UINT32);

	vulkanCommandBuffer->DrawIndexed(3, 1, 0, 0, 1);

	vulkanCommandBuffer->EndRenderPass();

	vulkanCommandBuffer->End();
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
