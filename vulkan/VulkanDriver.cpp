#include "VulkanDriver.h"

#include "DeviceProperties.h"

#include "VKInstance.h"
#include "VKSurface.h"
#include "VKDevice.h"
#include "VKSwapChain.h"

#include "VKCommandPool.h"
#include "VKCommandBuffer.h"

#include "VKSemaphore.h"
#include "VKFence.h"

#include "VKBuffer.h"
#include "VKImage.h"
#include "VKSampler.h"

#include "DescriptorSetMgr.h"

#include "VKShaderModule.h"
#include "VKPipelineLayout.h"
#include "VKPipeline.h"
#include "VKRenderPass.h"
#include "VKFramebuffer.h"

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
	ReleaseDeviceProperties();

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
	m_StagingBuffer = CreateVKBuffer(m_StagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
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

void VulkanDriver::QueueSubmit(VkSubmitInfo & submitInfo, VKFence * vkFence)
{
	VK_CHECK_RESULT(vkQueueSubmit(m_VKDevice->queue, 1, &submitInfo, vkFence->fence));
}

void VulkanDriver::AcquireNextImage(VKSemaphore * vkSemaphore)
{
	VkResult result = vkAcquireNextImageKHR(m_VKDevice->device, m_VKSwapChain->swapChain, UINT64_MAX, vkSemaphore->semaphore, VK_NULL_HANDLE, &m_ImageIndex);

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
}

void VulkanDriver::QueuePresent(VKSemaphore * vkSemaphore)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &vkSemaphore->semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_VKSwapChain->swapChain;
	presentInfo.pImageIndices = &m_ImageIndex;
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

VkImageView VulkanDriver::GetSwapChainCurrImageView()
{
	return m_VKSwapChain->swapChainImageViews[m_ImageIndex];
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
	return new VKCommandBuffer(m_VKDevice, vkCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}

VKSemaphore * VulkanDriver::CreateVKSemaphore()
{
	return new VKSemaphore(m_VKDevice);
}

VKFence * VulkanDriver::CreateVKFence(bool signaled)
{
	return new VKFence(m_VKDevice, signaled);
}

VKBuffer * VulkanDriver::CreateVKBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty)
{
	return new VKBuffer(m_VKDevice, size, usage, memoryProperty);
}

VKImage * VulkanDriver::CreateVKImage(VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI)
{
	return new VKImage(m_VKDevice, imageCI, viewCI);
}

VKSampler * VulkanDriver::CreateVKSampler(VkSamplerCreateInfo & ci)
{
	return new VKSampler(m_VKDevice, ci);
}

void VulkanDriver::UploadVKBuffer(VKBuffer * vkBuffer, void * data, VkDeviceSize size)
{
	m_StagingBuffer->Copy(data, 0, size);

	m_UploadVKCommandBuffer->Begin();

	VkBufferCopy bufferCopyInfo = {};
	bufferCopyInfo.srcOffset = 0;
	bufferCopyInfo.dstOffset = 0;
	bufferCopyInfo.size = size;
	m_UploadVKCommandBuffer->CopyVKBuffer(m_StagingBuffer, vkBuffer, bufferCopyInfo);

	// 经测试发现没有这一步也没问题（许多教程也的确没有这一步）
	// 个人认为是因为调用了WaitIdle
	//vkCmdPipelineBarrier

	m_UploadVKCommandBuffer->End();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_UploadVKCommandBuffer->commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	VK_CHECK_RESULT(vkQueueSubmit(m_VKDevice->queue, 1, &submitInfo, VK_NULL_HANDLE));

	DeviceWaitIdle();
}

void VulkanDriver::UploadVKImage(VKImage * image, void * data, VkDeviceSize size)
{
	m_StagingBuffer->Copy(data, 0, size);

	m_UploadVKCommandBuffer->Begin();

	m_UploadVKCommandBuffer->ImageMemoryBarrier(image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	m_UploadVKCommandBuffer->CopyVKBufferToVKImage(m_StagingBuffer, image);

	m_UploadVKCommandBuffer->ImageMemoryBarrier(image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	m_UploadVKCommandBuffer->End();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_UploadVKCommandBuffer->commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	VK_CHECK_RESULT(vkQueueSubmit(m_VKDevice->queue, 1, &submitInfo, VK_NULL_HANDLE));

	DeviceWaitIdle();
}

DescriptorSetMgr& VulkanDriver::GetDescriptorSetMgr()
{
	return *m_DescriptorSetMgr;
}

VKShaderModule * VulkanDriver::CreateVKShaderModule(const std::string & filename)
{
	return new VKShaderModule(m_VKDevice, filename);
}

VKPipelineLayout * VulkanDriver::CreateVKPipelineLayout(VkDescriptorSetLayout layout, VkShaderStageFlags pcStage, uint32_t pcSize)
{
	return new VKPipelineLayout(m_VKDevice, layout, pcStage, pcSize);
}

VKPipeline * VulkanDriver::CreateVKPipeline(PipelineCI & pipelineCI)
{
	return new VKPipeline(m_VKDevice, pipelineCI);;
}

VKRenderPass * VulkanDriver::CreateVKRenderPass(VkFormat colorFormat, VkFormat depthFormat)
{
	return new VKRenderPass(m_VKDevice, colorFormat, depthFormat);
}

VKFramebuffer* VulkanDriver::CreateVKFramebuffer(VKRenderPass* vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height)
{
	return new VKFramebuffer(m_VKDevice, vkRenderPass, color, depth, width, height);
}