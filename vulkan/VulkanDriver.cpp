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
#include "VKImage.h"
#include "VKSampler.h"

#include "DescriptorSetMgr.h"

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
	RELEASE(m_DescriptorSetMgr);

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

	m_DescriptorSetMgr = new DescriptorSetMgr(m_VulkanDevice->m_LogicalDevice);
}

void VulkanDriver::WaitIdle()
{
	m_VulkanDevice->WaitIdle();
}

uint32_t VulkanDriver::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties)
{
	return m_VulkanDevice->GetMemoryTypeIndex(typeBits, properties);
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

VKImage * VulkanDriver::CreateVKImage(VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI)
{
	return new VKImage(m_VulkanDevice->m_LogicalDevice, imageCI, viewCI);
}

VKSampler * VulkanDriver::CreateVKSampler(VkSamplerCreateInfo & ci)
{
	return new VKSampler(m_VulkanDevice->m_LogicalDevice, ci);
}

void VulkanDriver::UploadVulkanBuffer(VulkanBuffer * vertexBuffer, void * data, uint32_t size)
{
	m_UploadVulkanCommandBuffer->UploadVulkanBuffer(vertexBuffer, data, size, m_StagingBuffer);
}

void VulkanDriver::UploadVKImage(VKImage * image, void * data, uint32_t size)
{
	m_UploadVulkanCommandBuffer->UploadVKImage(image, data, size, m_StagingBuffer);
}

DescriptorSetMgr& VulkanDriver::GetDescriptorSetMgr()
{
	return *m_DescriptorSetMgr;
}

VulkanShaderModule * VulkanDriver::CreateVulkanShaderModule(const std::string & filename)
{
	return new VulkanShaderModule(m_VulkanDevice, filename);
}

VulkanPipelineLayout * VulkanDriver::CreateVulkanPipelineLayout(VkDescriptorSetLayout layout)
{
	return new VulkanPipelineLayout(m_VulkanDevice, layout);
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