#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "VulkanDescriptorTypes.h"

class VulkanDriver : public NonCopyable
{

	struct FrameResource {
		VulkanFramebuffer* framebuffer;
		VulkanCommandBuffer* commandBuffer;
		VulkanSemaphore* imageAvailableSemaphore;
		VulkanSemaphore* finishedRenderingSemaphore;
		VulkanFence* fence;

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

	// Device
	void WaitIdle();
	VkFormat GetDepthFormat();

	// SwapChain
	VkImageView GetSwapChainCurrImageView();
	uint32_t GetSwapChainWidth();
	uint32_t GetSwapChainHeight();
	VkFormat GetSwapChainFormat();

	// Command
	VulkanCommandPool* CreateVulkanCommandPool();
	VulkanCommandBuffer* CreateVulkanCommandBuffer(VulkanCommandPool* vulkanCommandPool);
	VulkanCommandBuffer* GetCurrVulkanCommandBuffer();

	// Semaphore
	VulkanSemaphore* CreateVulkanSemaphore();
	VulkanFence* CreateVulkanFence(bool signaled);

	// Resource
	VulkanBuffer* CreateVulkanBuffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	VulkanImage* CreateVulkanImage(VkImageType imageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage, VkImageAspectFlags aspect);
	void UploadVulkanBuffer(VulkanBuffer* vertexBuffer, void* data, uint32_t size);
	void UploadVulkanImage(VulkanImage* vulkanImage, void* data, uint32_t size);

	// Descriptor Set
	VulkanDescriptorSetLayout* CreateVulkanDescriptorSetLayout(DSLBindings& bindings);
	VulkanDescriptorPool* CreateVulkanDescriptorPool(uint32_t maxSets, DPSizes& sizes);
	void UpdateDescriptorSets(VulkanDescriptorSet* vulkanDescriptorSet, DesUpdateInfos& infos);

	// Render Status
	VulkanShaderModule* CreateVulkanShaderModule(const std::string& filename);
	VulkanPipelineLayout* CreateVulkanPipelineLayout(VulkanDescriptorSetLayout* vulkanDescriptorSetLayout);
	VulkanPipeline* CreateVulkanPipeline(PipelineCI& pipelineCI);
	VulkanRenderPass* CreateVulkanRenderPass(VkFormat colorFormat, VkFormat depthFormat);

	//
	VulkanFramebuffer* CreateFramebuffer(VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);
	VulkanFramebuffer* RebuildCurrFramebuffer(VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);

	//
	void UpdatePassUniformBuffer(void* data);
	void UpdateObjectUniformBuffer(void* data, uint32_t index);
	VulkanBuffer* GetCurrPassUniformBuffer();
	VulkanBuffer* GetCurrObjectUniformBuffer();

	// render
	void WaitForPresent();
	void Present();

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

	size_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;
	std::vector<VulkanBuffer*> m_PassUniformBuffers;
	const uint32_t m_ObjectUniformNum = 100;
	std::vector<VulkanBuffer*> m_ObjectUniformBuffers;

	VkFormat m_DepthFormat;
};

void CreateVulkanDriver();
VulkanDriver& GetVulkanDriver();
void ReleaseVulkanDriver();