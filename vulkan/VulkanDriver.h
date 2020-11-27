#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "DescriptorSetMgr.h"

class GPUProfilerMgr;
class UniformBufferMgr;
class Engine;

class VulkanDriver : public NonCopyable
{

	struct FrameResource {
		// ËÆºõ²»ÐèÒªVKFramebuffer£¬todo
		VKFramebuffer* framebuffer;
		VKCommandBuffer* commandBuffer;
		VKSemaphore* imageAvailableSemaphore;
		VKSemaphore* finishedRenderingSemaphore;
		VKFence* fence;

		FrameResource() :
			framebuffer(nullptr),
			commandBuffer(nullptr),
			imageAvailableSemaphore(nullptr),
			finishedRenderingSemaphore(nullptr),
			fence(nullptr) {
		}
	};

public:
	VulkanDriver();
	virtual ~VulkanDriver();

	void CleanUp();
	void Init();
	void Tick();

	// Device
	void DeviceWaitIdle();
	VkFormat GetSupportedDepthFormat();

	// SwapChain
	VkFormat GetSwapChainFormat();

	//
	void WaitForPresent();
	void Present();

	// Command
	VKCommandPool* CreateVKCommandPool();
	VKQueryPool* CreateVKQueryPool(VkQueryType queryType, uint32_t queryCount);
	VKCommandBuffer* CreateVKCommandBuffer(VKCommandPool* vkCommandPool);

	// Semaphore
	VKSemaphore* CreateVKSemaphore();
	VKFence* CreateVKFence(bool signaled);

	// Uniform Buffer
	void CreateUniformBuffer(std::string name, VkDeviceSize size);
	VKBuffer* GetUniformBuffer(std::string name);

	// Resource
	VKBuffer* CreateVKBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	VKImage* CreateVKImage();
	VKSampler* CreateVKSampler(VkSamplerCreateInfo& ci);
	void UploadVKBuffer(VKBuffer* vkBuffer, void* data, VkDeviceSize size);
	void UploadVKImage(VKImage* image, void* data, VkDeviceSize size);

	// DescriptorSet
	VkDescriptorSetLayout CreateDescriptorSetLayout(DSLBindings& bindings);
	VkDescriptorSet GetDescriptorSet(VkDescriptorSetLayout layout, bool persistent = false);
	void UpdateDescriptorSet(VkDescriptorSet set, DesUpdateInfos& infos);

	// GPUProfilerMgr
	void WriteTimeStamp(std::string name);
	std::string GetLastFrameGPUProfilerResult();

	// Render Status
	VKShaderModule* CreateVKShaderModule(const std::string& filename);
	VkPipelineLayout CreateVkPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts, VkShaderStageFlags pcStage = VK_SHADER_STAGE_VERTEX_BIT, uint32_t pcSize = 0);
	VKPipeline* CreateVKPipeline(PipelineCI& pipelineCI, VKPipeline* parent = nullptr);
	VKRenderPass* CreateVKRenderPass(VkFormat colorFormat);
	// temp
	void ReleaseVkPipelineLayout(VkPipelineLayout pipelineLayout);

	// Framebuffer
	VKFramebuffer* CreateVKFramebuffer(VKRenderPass* vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);
	VKFramebuffer* RebuildFramebuffer(VKRenderPass* vkRenderPass);

	// todo
	VKCommandBuffer* GetCurrVKCommandBuffer();

private:

	void AcquireNextImage(VKSemaphore* vkSemaphore);

	void QueuePresent(VKSemaphore* vkSemaphore);

private:

	// base
	VKInstance* m_VKInstance;
	VKSurface* m_VKSurface;
	VKDevice* m_VKDevice;
	VKSwapChain* m_VKSwapChain;
	VKCommandPool* m_VKCommandPool;

	// frame resources
	uint32_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	VKCommandBuffer* m_UploadVKCommandBuffer;
	const uint32_t m_StagingBufferSize = 10000000;
	VKBuffer* m_StagingBuffer;

	DescriptorSetMgr* m_DescriptorSetMgr;

	UniformBufferMgr* m_UniformBufferMgr;

	GPUProfilerMgr *m_GPUProfilerMgr;

	uint32_t m_ImageIndex;

	// depth
	VkFormat m_DepthFormat;
	VKImage* m_DepthImage;
};

void CreateVulkanDriver();
VulkanDriver& GetVulkanDriver();
void ReleaseVulkanDriver();