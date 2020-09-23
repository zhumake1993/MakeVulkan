#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "DescriptorSetMgr.h"

class VulkanDriver : public NonCopyable
{

public:
	VulkanDriver();
	virtual ~VulkanDriver();

	void CleanUp();
	void Init();

	// Device
	void WaitIdle();
	VkFormat GetDepthFormat();
	void QueueSubmit(VkSubmitInfo& submitInfo, VulkanFence* fence);

	// SwapChain
	void AcquireNextImage(VulkanSemaphore* vulkanSemaphore);
	void QueuePresent(VulkanSemaphore* vulkanSemaphore);
	VkImageView GetSwapChainCurrImageView();
	uint32_t GetSwapChainWidth();
	uint32_t GetSwapChainHeight();
	VkFormat GetSwapChainFormat();

	// Command
	VulkanCommandPool* CreateVulkanCommandPool();
	VulkanCommandBuffer* CreateVulkanCommandBuffer(VulkanCommandPool* vulkanCommandPool);

	// Semaphore
	VulkanSemaphore* CreateVulkanSemaphore();
	VulkanFence* CreateVulkanFence(bool signaled);

	// Resource
	VulkanBuffer* CreateVulkanBuffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	VulkanImage* CreateVulkanImage(VkImageType imageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage, VkImageAspectFlags aspect);
	void UploadVulkanBuffer(VulkanBuffer* vertexBuffer, void* data, uint32_t size);
	void UploadVulkanImage(VulkanImage* vulkanImage, void* data, uint32_t size);

	// DescriptorSetMgr
	DescriptorSetMgr& GetDescriptorSetMgr();

	// Render Status
	VulkanShaderModule* CreateVulkanShaderModule(const std::string& filename);
	VulkanPipelineLayout* CreateVulkanPipelineLayout(VkDescriptorSetLayout layout);
	VulkanPipeline* CreateVulkanPipeline(PipelineCI& pipelineCI);
	VulkanRenderPass* CreateVulkanRenderPass(VkFormat colorFormat, VkFormat depthFormat);

	// Framebuffer
	VulkanFramebuffer* CreateFramebuffer(VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);

	// ½ö×ö²âÊÔÓÃ£¡£¡
	VulkanDevice* GetVulkanDevice() {
		return m_VulkanDevice;
	}

private:

	//

private:

	VulkanInstance* m_VulkanInstance;
	VulkanSurface* m_VulkanSurface;
	VulkanDevice* m_VulkanDevice;
	VulkanSwapChain* m_VulkanSwapChain;

	VulkanCommandPool* m_VulkanCommandPool;
	VulkanCommandBuffer* m_UploadVulkanCommandBuffer;
	const uint32_t m_StagingBufferSize = 10000000;
	VulkanBuffer* m_StagingBuffer;

	VkFormat m_DepthFormat;

	DescriptorSetMgr* m_DescriptorSetMgr;
};

void CreateVulkanDriver();
VulkanDriver& GetVulkanDriver();
void ReleaseVulkanDriver();