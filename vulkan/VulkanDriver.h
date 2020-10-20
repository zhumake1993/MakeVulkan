#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "DescriptorSetMgr.h"

class GPUProfilerMgr;

class VulkanDriver : public NonCopyable
{

public:
	VulkanDriver();
	virtual ~VulkanDriver();

	void CleanUp();
	void Init();

	// Device
	void DeviceWaitIdle();
	uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);
	VkFormat GetSupportedDepthFormat();
	void QueueSubmit(VkSubmitInfo& submitInfo, VKFence* vkFence);

	// SwapChain
	void AcquireNextImage(VKSemaphore* vkSemaphore);
	void QueuePresent(VKSemaphore* vkSemaphore);
	VkImageView GetSwapChainCurrImageView();
	uint32_t GetSwapChainWidth();
	uint32_t GetSwapChainHeight();
	VkFormat GetSwapChainFormat();

	// Command
	VKCommandPool* CreateVKCommandPool();
	VKQueryPool* CreateVKQueryPool(VkQueryType queryType, uint32_t queryCount);
	VKCommandBuffer* CreateVKCommandBuffer(VKCommandPool* vkCommandPool);

	// Semaphore
	VKSemaphore* CreateVKSemaphore();
	VKFence* CreateVKFence(bool signaled);

	// Resource
	VKBuffer* CreateVKBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	VKImage* CreateVKImage(VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI);
	VKSampler* CreateVKSampler(VkSamplerCreateInfo& ci);
	void UploadVKBuffer(VKBuffer* vkBuffer, void* data, VkDeviceSize size);
	void UploadVKImage(VKImage* image, void* data, VkDeviceSize size);

	// DescriptorSetMgr
	DescriptorSetMgr& GetDescriptorSetMgr();

	// GPUProfilerMgr
	GPUProfilerMgr* CreateGPUProfilerMgr();

	// Render Status
	VKShaderModule* CreateVKShaderModule(const std::string& filename);
	VKPipelineLayout* CreateVKPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts, VkShaderStageFlags pcStage = VK_SHADER_STAGE_VERTEX_BIT, uint32_t pcSize = 0);
	VKPipeline* CreateVKPipeline(PipelineCI& pipelineCI, VKPipeline* parent = nullptr);
	VKRenderPass* CreateVKRenderPass(VkFormat colorFormat, VkFormat depthFormat);

	// Framebuffer
	VKFramebuffer* CreateVKFramebuffer(VKRenderPass* vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);

private:

	//

private:

	VKInstance* m_VKInstance;
	VKSurface* m_VKSurface;
	VKDevice* m_VKDevice;
	VKSwapChain* m_VKSwapChain;

	VKCommandPool* m_VKCommandPool;
	VKCommandBuffer* m_UploadVKCommandBuffer;
	const uint32_t m_StagingBufferSize = 10000000;
	VKBuffer* m_StagingBuffer;

	DescriptorSetMgr* m_DescriptorSetMgr;

	uint32_t m_ImageIndex;
};

void CreateVulkanDriver();
VulkanDriver& GetVulkanDriver();
void ReleaseVulkanDriver();