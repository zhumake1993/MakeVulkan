#include "VulkanDriver.h"

#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"

#include "VulkanSemaphore.h"
#include "VulkanFence.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"

#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

#include "VulkanFramebuffer.h"

#include "Tools.h"

VulkanDriver* driver;

void CreateVulkanDriver()
{
	driver = new VulkanDriver();
	driver->Init();
}

VulkanDriver& GetVulkanDriver()
{
	return *driver;
}

void ReleaseVulkanDriver()
{
	driver->CleanUp();
	RELEASE(driver);
}

VulkanDriver::VulkanDriver()
{
}

VulkanDriver::~VulkanDriver()
{
	// 不要在这里释放资源！
}

void VulkanDriver::CleanUp()
{
	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		RELEASE(m_FrameResources[i].framebuffer);
		RELEASE(m_FrameResources[i].commandBuffer);
		RELEASE(m_FrameResources[i].imageAvailableSemaphore);
		RELEASE(m_FrameResources[i].finishedRenderingSemaphore);
		RELEASE(m_FrameResources[i].fence);
	}

	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		RELEASE(m_PassUniformBuffers[i]);
		RELEASE(m_ObjectUniformBuffers[i]);
	}

	RELEASE(m_UploadVulkanCommandBuffer);
	RELEASE(m_VulkanCommandPool);
	RELEASE(m_StagingBuffer);

	RELEASE(m_VulkanSwapChain);
	RELEASE(m_VulkanDevice);
	RELEASE(m_VulkanSurface);
	RELEASE(m_VulkanInstance);
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

	m_VulkanCommandPool = CreateVulkanCommandPool();
	m_UploadVulkanCommandBuffer = CreateVulkanCommandBuffer(m_VulkanCommandPool);
	m_StagingBuffer = CreateVulkanBuffer(m_StagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	m_FrameResources.resize(global::frameResourcesCount);
	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_FrameResources[i].framebuffer = nullptr; // 动态创建framebuffer
		m_FrameResources[i].commandBuffer = m_VulkanCommandPool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		m_FrameResources[i].imageAvailableSemaphore = CreateVulkanSemaphore();
		m_FrameResources[i].finishedRenderingSemaphore = CreateVulkanSemaphore();
		m_FrameResources[i].fence = CreateVulkanFence(true);
	}

	m_PassUniformBuffers.resize(global::frameResourcesCount);
	m_ObjectUniformBuffers.resize(global::frameResourcesCount);
	for (size_t i = 0; i < global::frameResourcesCount; ++i) {
		m_PassUniformBuffers[i] = CreateVulkanBuffer(sizeof(PassUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_ObjectUniformBuffers[i] = CreateVulkanBuffer(sizeof(ObjectUniform) * m_ObjectUniformNum, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	m_DepthFormat = m_VulkanDevice->GetSupportedDepthFormat();
}

void VulkanDriver::WaitIdle()
{
	m_VulkanDevice->WaitIdle();
}

VkFormat VulkanDriver::GetDepthFormat()
{
	return m_DepthFormat;
}

VkImageView VulkanDriver::GetSwapChainCurrImageView()
{
	return m_VulkanSwapChain->GetCurrImageView();
}

uint32_t VulkanDriver::GetSwapChainWidth()
{
	return m_VulkanSwapChain->m_Extent.width;
}

uint32_t VulkanDriver::GetSwapChainHeight()
{
	return m_VulkanSwapChain->m_Extent.height;
}

VkFormat VulkanDriver::GetSwapChainFormat()
{
	return m_VulkanSwapChain->GetFormat();
}

VulkanCommandPool * VulkanDriver::CreateVulkanCommandPool()
{
	return new VulkanCommandPool(m_VulkanDevice, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_VulkanDevice->m_SelectedQueueFamilyIndex);;
}

VulkanCommandBuffer * VulkanDriver::CreateVulkanCommandBuffer(VulkanCommandPool * vulkanCommandPool)
{
	return vulkanCommandPool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}

VulkanCommandBuffer * VulkanDriver::GetCurrVulkanCommandBuffer()
{
	return m_FrameResources[m_CurrFrameIndex].commandBuffer;
}

VulkanSemaphore * VulkanDriver::CreateVulkanSemaphore()
{
	return new VulkanSemaphore(m_VulkanDevice);
}

VulkanFence * VulkanDriver::CreateVulkanFence(bool signaled)
{
	return new VulkanFence(m_VulkanDevice, signaled);
}

VulkanBuffer * VulkanDriver::CreateVulkanBuffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty)
{
	return new VulkanBuffer(m_VulkanDevice, size, usage, memoryProperty);
}

VulkanImage * VulkanDriver::CreateVulkanImage(VkImageType imageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage, VkImageAspectFlags aspect)
{
	return new VulkanImage(m_VulkanDevice, imageType, format, width, height, usage, aspect);
}

void VulkanDriver::UploadVulkanBuffer(VulkanBuffer * vertexBuffer, void * data, uint32_t size)
{
	m_UploadVulkanCommandBuffer->UploadVulkanBuffer(vertexBuffer, data, size, m_StagingBuffer);
}

void VulkanDriver::UploadVulkanImage(VulkanImage * vulkanImage, void * data, uint32_t size)
{
	m_UploadVulkanCommandBuffer->UploadVulkanImage(vulkanImage, data, size, m_StagingBuffer);
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

VulkanRenderPass * VulkanDriver::CreateVulkanRenderPass(VkFormat colorFormat, VkFormat depthFormat)
{
	return new VulkanRenderPass(m_VulkanDevice, colorFormat, depthFormat);
}

VulkanFramebuffer* VulkanDriver::CreateFramebuffer(VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height)
{
	return new VulkanFramebuffer(m_VulkanDevice, vulkanRenderPass, color, depth, width, height);
}

VulkanFramebuffer * VulkanDriver::RebuildCurrFramebuffer(VulkanRenderPass * vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height)
{
	RELEASE(m_FrameResources[m_CurrFrameIndex].framebuffer);

	m_FrameResources[m_CurrFrameIndex].framebuffer = CreateFramebuffer(vulkanRenderPass, color, depth, width, height);

	return m_FrameResources[m_CurrFrameIndex].framebuffer;
}

void VulkanDriver::UpdatePassUniformBuffer(void * data)
{
	m_PassUniformBuffers[m_CurrFrameIndex]->MapAndCopy(data, sizeof(PassUniform));
}

void VulkanDriver::UpdateObjectUniformBuffer(void * data, uint32_t index)
{
	//
}

VulkanBuffer * VulkanDriver::GetCurrPassUniformBuffer()
{
	return m_PassUniformBuffers[m_CurrFrameIndex];
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