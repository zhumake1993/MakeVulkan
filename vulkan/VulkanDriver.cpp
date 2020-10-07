#include "VulkanDriver.h"

#include "DeviceProperties.h"

#include "VKInstance.h"
#include "VKSurface.h"
#include "VKDevice.h"
#include "VKSwapChain.h"

#include "VKCommandPool.h"
#include "VKCommandBuffer.h"

#include "VulkanSemaphore.h"
#include "VulkanFence.h"

#include "VulkanBuffer.h"
#include "VKImage.h"
#include "VKSampler.h"

#include "DescriptorSetMgr.h"

#include "VulkanShaderModule.h"
#include "VKPipelineLayout.h"
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

	RELEASE(m_UploadVKCommandBuffer);
	RELEASE(m_VKCommandPool);
	RELEASE(m_StagingBuffer);

	RELEASE(m_VKSwapChain);
	RELEASE(m_VKDevice);
	RELEASE(m_VKSurface);
	RELEASE(m_VKInstance);
}

void VulkanDriver::Init()
{
	m_VKInstance = new VKInstance();

#if defined(_WIN32)
	m_VKSurface = new VKSurface(m_VKInstance, global::windowInstance, global::windowHandle);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	m_VKSurface = new VKSurface(m_VKInstance, androidApp->window);
#endif

	m_VKDevice = new VKDevice(m_VKInstance, m_VKSurface);
	m_VKSwapChain = new VKSwapChain(m_VKDevice, m_VKSurface);

	m_VKCommandPool = CreateVKCommandPool();
	m_UploadVKCommandBuffer = CreateVKCommandBuffer(m_VKCommandPool);
	m_StagingBuffer = CreateVulkanBuffer(m_StagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	m_StagingBuffer->Map();

	m_DescriptorSetMgr = new DescriptorSetMgr(m_VKDevice->device);
}

void VulkanDriver::DeviceWaitIdle()
{
	vkDeviceWaitIdle(m_VKDevice->device);
}

uint32_t VulkanDriver::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties)
{
	auto& dp = GetDeviceProperties();

	for (uint32_t i = 0; i < dp.deviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((dp.deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		typeBits >>= 1;
	}

	LOG("Could not find a matching memory type");
	assert(false);
	return 0;
}

VkFormat VulkanDriver::GetSupportedDepthFormat()
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	std::vector<VkFormat> depthFormats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	for (auto& format : depthFormats)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_VKDevice->physicalDevice, format, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			return format;
		}
	}

	LOG("Can not find supported depth format");
	return VK_FORMAT_UNDEFINED;
}

void VulkanDriver::QueueSubmit(VkSubmitInfo & submitInfo, VulkanFence * fence)
{
	VK_CHECK_RESULT(vkQueueSubmit(m_VKDevice->queue, 1, &submitInfo, fence->m_Fence));
}

uint32_t VulkanDriver::AcquireNextImage(VulkanSemaphore * vulkanSemaphore)
{
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_VKDevice->device, m_VKSwapChain->swapChain, UINT64_MAX, vulkanSemaphore->m_Semaphore, VK_NULL_HANDLE, &imageIndex);

	switch (result) {
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		LOG("recreate swapchain\n");
		RELEASE(m_VKSwapChain);
		m_VKSwapChain = new VKSwapChain(m_VKDevice, m_VKSurface);
		break;
	default:
		LOG("Problem occurred during swap chain image acquisition!\n");
		assert(false);
	}

	return imageIndex;
}

void VulkanDriver::QueuePresent(VulkanSemaphore * vulkanSemaphore, uint32_t imageIndex)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &vulkanSemaphore->m_Semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_VKSwapChain->swapChain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(m_VKDevice->queue, &presentInfo);

	switch (result) {
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		LOG("recreate swapchain\n");
		RELEASE(m_VKSwapChain);
		m_VKSwapChain = new VKSwapChain(m_VKDevice, m_VKSurface);
		break;
	default:
		LOG("Problem occurred during image presentation!\n");
		assert(false);
	}
}

VkImageView VulkanDriver::GetSwapChainImageView(uint32_t imageIndex)
{
	return m_VKSwapChain->swapChainImageViews[imageIndex];
}

uint32_t VulkanDriver::GetSwapChainWidth()
{
	return m_VKSwapChain->extent.width;
}

uint32_t VulkanDriver::GetSwapChainHeight()
{
	return m_VKSwapChain->extent.height;
}

VkFormat VulkanDriver::GetSwapChainFormat()
{
	return m_VKSwapChain->format.format;
}

VKCommandPool * VulkanDriver::CreateVKCommandPool()
{
	auto& dp = GetDeviceProperties();

	return new VKCommandPool(m_VKDevice, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, dp.selectedQueueFamilyIndex);;
}

VKCommandBuffer * VulkanDriver::CreateVKCommandBuffer(VKCommandPool * vkCommandPool)
{
	VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.pNext = nullptr;
	cmdBufferAllocInfo.commandPool = vkCommandPool->commandPool;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocInfo.commandBufferCount = 1;

	VKCommandBuffer* vkCommandBuffer = new VKCommandBuffer(m_VKDevice, vkCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	VK_CHECK_RESULT(vkAllocateCommandBuffers(m_VKDevice->device, &cmdBufferAllocInfo, &vkCommandBuffer->commandBuffer));
	return vkCommandBuffer;
}

VulkanSemaphore * VulkanDriver::CreateVulkanSemaphore()
{
	return new VulkanSemaphore(m_VKDevice->device);
}

VulkanFence * VulkanDriver::CreateVulkanFence(bool signaled)
{
	return new VulkanFence(m_VKDevice->device, signaled);
}

VulkanBuffer * VulkanDriver::CreateVulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty)
{
	return new VulkanBuffer(m_VKDevice->device, size, usage, memoryProperty);
}

VKImage * VulkanDriver::CreateVKImage(VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI)
{
	return new VKImage(m_VKDevice->device, imageCI, viewCI);
}

VKSampler * VulkanDriver::CreateVKSampler(VkSamplerCreateInfo & ci)
{
	return new VKSampler(m_VKDevice->device, ci);
}

void VulkanDriver::UploadVulkanBuffer(VulkanBuffer * vertexBuffer, void * data, VkDeviceSize size)
{
	m_UploadVulkanCommandBuffer->UploadVulkanBuffer(vertexBuffer, data, size, m_StagingBuffer);
}

void VulkanDriver::UploadVKImage(VKImage * image, void * data, VkDeviceSize size)
{
	m_UploadVulkanCommandBuffer->UploadVKImage(image, data, size, m_StagingBuffer);
}

DescriptorSetMgr& VulkanDriver::GetDescriptorSetMgr()
{
	return *m_DescriptorSetMgr;
}

VulkanShaderModule * VulkanDriver::CreateVulkanShaderModule(const std::string & filename)
{
	return new VulkanShaderModule(m_VKDevice->device, filename);
}

VKPipelineLayout * VulkanDriver::CreateVKPipelineLayout(VkDescriptorSetLayout layout, VkShaderStageFlags pcStage, uint32_t pcSize)
{
	return new VKPipelineLayout(m_VKDevice->device, layout, pcStage, pcSize);
}

VulkanPipeline * VulkanDriver::CreateVulkanPipeline(PipelineCI & pipelineCI)
{
	return new VulkanPipeline(m_VKDevice->device, pipelineCI);;
}

VulkanRenderPass * VulkanDriver::CreateVulkanRenderPass(VkFormat colorFormat, VkFormat depthFormat)
{
	return new VulkanRenderPass(m_VKDevice->device, colorFormat, depthFormat);
}

VulkanFramebuffer* VulkanDriver::CreateFramebuffer(VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height)
{
	return new VulkanFramebuffer(m_VKDevice->device, vulkanRenderPass, color, depth, width, height);
}