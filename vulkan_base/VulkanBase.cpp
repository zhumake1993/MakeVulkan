#include "VulkanBase.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
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

	m_StagingBuffer->CleanUp();

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

	m_StagingBuffer = new VulkanBuffer(m_VulkanDevice, 1000000, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

void VulkanBase::UploadBuffer(VulkanBuffer * vertexBuffer, void * data, uint32_t size)
{
	m_StagingBuffer->MapAndCopy(data, size);

	auto cmd = m_VulkanCommandPool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	cmd->Begin();

	VkBufferCopy bufferCopyInfo = {};
	bufferCopyInfo.srcOffset = 0;
	bufferCopyInfo.dstOffset = 0;
	bufferCopyInfo.size = size;
	cmd->CopyBuffer(m_StagingBuffer, vertexBuffer, bufferCopyInfo);

	// 经测试发现没有这一步也没问题（许多教程也的确没有这一步）
	//vkCmdPipelineBarrier

	cmd->End();

	m_VulkanDevice->Submit(cmd);

	m_VulkanDevice->WaitIdle();

	cmd->CleanUp();
}

void VulkanBase::UploadImage(VulkanImage * vulkanImage, void * data, uint32_t size)
{
	m_StagingBuffer->MapAndCopy(data, size);

	auto cmd = m_VulkanCommandPool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	cmd->Begin();

	VkImageSubresourceRange imageSubresourceRange = {};
	imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageSubresourceRange.baseMipLevel = 0;
	imageSubresourceRange.levelCount = 1;;
	imageSubresourceRange.baseArrayLayer = 0;
	imageSubresourceRange.layerCount = 1;

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = 0;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = vulkanImage->m_Image;
	imageMemoryBarrier.subresourceRange = imageSubresourceRange;

	vkCmdPipelineBarrier(cmd->m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	VkBufferImageCopy bufferImageCopyInfo = {};
	bufferImageCopyInfo.bufferOffset = 0;
	bufferImageCopyInfo.bufferRowLength = 0;
	bufferImageCopyInfo.bufferImageHeight = 0;
	bufferImageCopyInfo.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT,0,0,1 };
	bufferImageCopyInfo.imageOffset = { 0,0,0 };
	bufferImageCopyInfo.imageExtent = { vulkanImage->m_Width,vulkanImage->m_Height,1 };

	vkCmdCopyBufferToImage(cmd->m_CommandBuffer, m_StagingBuffer->m_Buffer, vulkanImage->m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyInfo);

	VkImageMemoryBarrier imageMemoryBarrier2 = {};
	imageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier2.pNext = nullptr;
	imageMemoryBarrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageMemoryBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageMemoryBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier2.image = vulkanImage->m_Image;
	imageMemoryBarrier2.subresourceRange = imageSubresourceRange;

	vkCmdPipelineBarrier(cmd->m_CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier2);

	cmd->End();

	m_VulkanDevice->Submit(cmd);

	m_VulkanDevice->WaitIdle();

	cmd->CleanUp();
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
