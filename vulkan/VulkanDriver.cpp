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
	// ��Ҫ�������ͷ���Դ��
}

void VulkanDriver::CleanUp()
{
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
	m_StagingBuffer->Map();

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

void VulkanDriver::QueueSubmit(VkSubmitInfo & submitInfo, VulkanFence * fence)
{
	VK_CHECK_RESULT(vkQueueSubmit(m_VulkanDevice->m_Queue, 1, &submitInfo, fence->m_Fence));
}

void VulkanDriver::AcquireNextImage(VulkanSemaphore * vulkanSemaphore)
{
	m_VulkanSwapChain->AcquireNextImage(vulkanSemaphore);
}

void VulkanDriver::QueuePresent(VulkanSemaphore * vulkanSemaphore)
{
	m_VulkanSwapChain->QueuePresent(vulkanSemaphore);
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

VulkanDescriptorSetLayout * VulkanDriver::CreateVulkanDescriptorSetLayout(DSLBindings& bindings)
{
	return new VulkanDescriptorSetLayout(m_VulkanDevice, bindings);
}

VulkanDescriptorPool * VulkanDriver::CreateVulkanDescriptorPool(uint32_t maxSets, DPSizes& sizes)
{
	return new VulkanDescriptorPool(m_VulkanDevice, maxSets, sizes);
}

void VulkanDriver::UpdateDescriptorSets(VulkanDescriptorSet * vulkanDescriptorSet, DesUpdateInfos& infos)
{
	uint32_t num = static_cast<uint32_t>(infos.size());

	std::vector<VkWriteDescriptorSet> writeDescriptorSets(num);
	for (uint32_t i = 0; i < num; i++) {
		writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[i].pNext = nullptr;
		writeDescriptorSets[i].dstSet = vulkanDescriptorSet->m_DescriptorSet;
		writeDescriptorSets[i].dstBinding = infos[i].binding;
		writeDescriptorSets[i].dstArrayElement = 0;
		writeDescriptorSets[i].descriptorCount = 1;
		writeDescriptorSets[i].descriptorType = vulkanDescriptorSet->GetDescriptorType(infos[i].binding);
		writeDescriptorSets[i].pImageInfo = &infos[i].info.image;
		writeDescriptorSets[i].pBufferInfo = &infos[i].info.buffer;
		writeDescriptorSets[i].pTexelBufferView = &infos[i].info.texelBufferView;
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