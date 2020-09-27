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
	uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);
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
	VulkanBuffer* CreateVulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	VKImage* CreateVKImage(VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI);
	VKSampler* CreateVKSampler(VkSamplerCreateInfo& ci);
	void UploadVulkanBuffer(VulkanBuffer* vertexBuffer, void* data, VkDeviceSize size);
	void UploadVKImage(VKImage* image, void* data, VkDeviceSize size);

	// DescriptorSetMgr
	DescriptorSetMgr& GetDescriptorSetMgr();

	// Render Status
	VulkanShaderModule* CreateVulkanShaderModule(const std::string& filename);
	VKPipelineLayout* CreateVKPipelineLayout(VkDescriptorSetLayout layout, VkShaderStageFlags pcStage = VK_SHADER_STAGE_VERTEX_BIT, uint32_t pcSize = 0);
	VulkanPipeline* CreateVulkanPipeline(PipelineCI& pipelineCI);
	VulkanRenderPass* CreateVulkanRenderPass(VkFormat colorFormat, VkFormat depthFormat);

	// Framebuffer
	VulkanFramebuffer* CreateFramebuffer(VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);

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