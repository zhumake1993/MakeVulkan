#pragma once

#include "Common.h"
#include "VulkanFwd.h"

class VulkanDriver
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
	void WaitIdle();
	void WaitForPresent();
	void Present();

	// Resource
	VulkanBuffer* CreateVulkanBuffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	VulkanImage* CreateVulkanImage(VkImageType imageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage);
	void UploadVulkanBuffer(VulkanBuffer* vertexBuffer, void* data, uint32_t size);
	void UploadVulkanImage(VulkanImage* vulkanImage, void* data, uint32_t size);

	// DescriptorSet
	VulkanDescriptorSetLayout* CreateVulkanDescriptorSetLayout();
	VulkanDescriptorPool* CreateVulkanDescriptorPool();
	void UpdateDescriptorSets(std::vector<DescriptorSetUpdater*>& descriptorSetUpdaters);

	// Pipeline
	VulkanShaderModule* CreateVulkanShaderModule(const std::string& filename);
	VulkanPipelineLayout* CreateVulkanPipelineLayout(VulkanDescriptorSetLayout* vulkanDescriptorSetLayout);
	VulkanPipeline* CreateVulkanPipeline(PipelineCI& pipelineCI);

	//
	void UpdateUniformBuffer(void* data, uint32_t size);

	// Ø½´ýÐÞ¸Ä
	VulkanFramebuffer* CreateFramebuffer();
	VulkanCommandBuffer* GetCurrCommandBuffer();
	VulkanBuffer* GetCurrUniformBuffer();
	VulkanRenderPass* GetRenderPass();

private:

	VulkanInstance* m_VulkanInstance;
	VulkanSurface* m_VulkanSurface;
	VulkanDevice* m_VulkanDevice;
	VulkanSwapChain* m_VulkanSwapChain;
	VulkanCommandPool* m_VulkanCommandPool;

	size_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	VulkanRenderPass* m_VulkanRenderPass;

	VulkanBuffer* m_StagingBuffer;

	std::vector<VulkanBuffer*> m_UniformBuffers;
};

void SetVulkanDriver(VulkanDriver* vulkanDriver);
VulkanDriver& GetVulkanDriver();