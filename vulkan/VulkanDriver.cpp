#include "VulkanDriver.h"

#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandPool.h"

#include "VulkanCommandBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "VulkanFramebuffer.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"

#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

#include "Tools.h"

VulkanDriver* driver;

void SetVulkanDriver(VulkanDriver * vulkanDriver)
{
	driver = vulkanDriver;
}

VulkanDriver& GetVulkanDriver()
{
	return *driver;
}

VulkanDriver::VulkanDriver()
{
}

VulkanDriver::~VulkanDriver()
{
}

void VulkanDriver::CleanUp()
{
	m_StagingBuffer->CleanUp();

	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_UniformBuffers[i]->CleanUp();

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

void VulkanDriver::Init()
{
	m_VulkanInstance = new VulkanInstance();

#if defined(_WIN32)
	m_VulkanSurface = new VulkanSurface(m_VulkanInstance, global::windowInstance, global::windowHandle);
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

	m_StagingBuffer = new VulkanBuffer(m_VulkanDevice, 1000000, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	uint32_t uniformBufferSize = sizeof(float) * 128;
	m_UniformBuffers.resize(global::frameResourcesCount);
	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_UniformBuffers[i] = new VulkanBuffer(m_VulkanDevice, uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

}

void VulkanDriver::WaitIdle()
{
	m_VulkanDevice->WaitIdle();
}

void VulkanDriver::WaitForPresent()
{
	auto& currFrameResource = m_FrameResources[m_CurrFrameIndex];

	currFrameResource.fence->Wait();
	currFrameResource.fence->Reset();

	m_VulkanSwapChain->AcquireNextImage(currFrameResource.imageAvailableSemaphore);
}

void VulkanDriver::Present()
{
	auto& currFrameResource = m_FrameResources[m_CurrFrameIndex];

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

	m_VulkanSwapChain->QueuePresent(currFrameResource.finishedRenderingSemaphore);

	m_CurrFrameIndex = (m_CurrFrameIndex + 1) % global::frameResourcesCount;
}

VulkanBuffer * VulkanDriver::CreateVulkanBuffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty)
{
	return new VulkanBuffer(m_VulkanDevice, size, usage, memoryProperty);
}

VulkanImage * VulkanDriver::CreateVulkanImage(VkImageType imageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage)
{
	return new VulkanImage(m_VulkanDevice, imageType, format, width, height, usage);
}

void VulkanDriver::UploadVulkanBuffer(VulkanBuffer * vertexBuffer, void * data, uint32_t size)
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
	// 个人认为是因为调用了WaitIdle
	//vkCmdPipelineBarrier

	cmd->End();

	m_VulkanDevice->Submit(cmd);

	m_VulkanDevice->WaitIdle();

	cmd->CleanUp();
}

void VulkanDriver::UploadVulkanImage(VulkanImage * vulkanImage, void * data, uint32_t size)
{
	m_StagingBuffer->MapAndCopy(data, size);

	auto cmd = m_VulkanCommandPool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	cmd->Begin();

	cmd->ImageMemoryBarrier(vulkanImage, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	cmd->CopyBufferToImage(m_StagingBuffer, vulkanImage);

	cmd->ImageMemoryBarrier(vulkanImage, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	cmd->End();

	m_VulkanDevice->Submit(cmd);

	m_VulkanDevice->WaitIdle();

	cmd->CleanUp();
}

VulkanDescriptorSetLayout * VulkanDriver::CreateVulkanDescriptorSetLayout()
{
	return new VulkanDescriptorSetLayout(m_VulkanDevice);
}

VulkanDescriptorPool * VulkanDriver::CreateVulkanDescriptorPool()
{
	return new VulkanDescriptorPool(m_VulkanDevice);
}

void VulkanDriver::UpdateDescriptorSets(std::vector<DescriptorSetUpdater*>& descriptorSetUpdaters)
{
	uint32_t num = static_cast<uint32_t>(descriptorSetUpdaters.size());

	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.reserve(num);

	for (auto updater : descriptorSetUpdaters) {
		writeDescriptorSets.push_back(updater->Get());
	}

	vkUpdateDescriptorSets(m_VulkanDevice->m_LogicalDevice, num, writeDescriptorSets.data(), 0, nullptr);
}

VulkanShaderModule * VulkanDriver::CreateVulkanShaderModule(const std::string & filename)
{
	return new VulkanShaderModule(m_VulkanDevice, filename);
}

VulkanPipelineLayout * VulkanDriver::CreateVulkanPipelineLayout(VulkanDescriptorSetLayout * vulkanDescriptorSetLayout)
{
	return new VulkanPipelineLayout(m_VulkanDevice, vulkanDescriptorSetLayout->m_DescriptorSetLayout);;
}

VulkanPipeline * VulkanDriver::CreateVulkanPipeline(PipelineCI & pipelineCI)
{
	return new VulkanPipeline(m_VulkanDevice, pipelineCI);;
}

VulkanRenderPass * VulkanDriver::CreateVulkanRenderPass()
{
	return new VulkanRenderPass(m_VulkanDevice, m_VulkanSwapChain->m_Format.format);
}

void VulkanDriver::UpdateUniformBuffer(void * data, uint32_t size)
{
	m_UniformBuffers[m_CurrFrameIndex]->MapAndCopy(data, size);
}

VulkanFramebuffer* VulkanDriver::CreateFramebuffer(VulkanRenderPass* vulkanRenderPass)
{
	auto& vulkanFramebuffer = m_FrameResources[m_CurrFrameIndex].framebuffer;

	if (vulkanFramebuffer->m_Framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(m_VulkanDevice->m_LogicalDevice, vulkanFramebuffer->m_Framebuffer, nullptr);
		vulkanFramebuffer->m_Framebuffer = VK_NULL_HANDLE;
	}

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.flags = 0;
	frameBufferCreateInfo.renderPass = vulkanRenderPass->m_RenderPass;
	frameBufferCreateInfo.attachmentCount = 1;
	frameBufferCreateInfo.pAttachments = m_VulkanSwapChain->GetCurrImageView();
	frameBufferCreateInfo.width = m_VulkanSwapChain->m_Extent.width;
	frameBufferCreateInfo.height = m_VulkanSwapChain->m_Extent.height;
	frameBufferCreateInfo.layers = 1;
	VK_CHECK_RESULT(vkCreateFramebuffer(m_VulkanDevice->m_LogicalDevice, &frameBufferCreateInfo, nullptr, &vulkanFramebuffer->m_Framebuffer));

	return vulkanFramebuffer;
}

VulkanCommandBuffer * VulkanDriver::GetCurrCommandBuffer()
{
	return m_FrameResources[m_CurrFrameIndex].commandBuffer;
}

VulkanBuffer * VulkanDriver::GetCurrUniformBuffer()
{
	return m_UniformBuffers[m_CurrFrameIndex];
}
