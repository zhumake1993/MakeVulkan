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
	VulkanImage* CreateVulkanImage(VkImageType imageType, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage, VkImageAspectFlags aspect);
	void UploadVulkanBuffer(VulkanBuffer* vertexBuffer, void* data, uint32_t size);
	void UploadVulkanImage(VulkanImage* vulkanImage, void* data, uint32_t size);

	// Descriptor Set
	VulkanDescriptorSetLayout* CreateVulkanDescriptorSetLayout();
	VulkanDescriptorPool* CreateVulkanDescriptorPool();
	void UpdateDescriptorSets(std::vector<DescriptorSetUpdater*>& descriptorSetUpdaters);

	// Render Status
	VulkanShaderModule* CreateVulkanShaderModule(const std::string& filename);
	VulkanPipelineLayout* CreateVulkanPipelineLayout(VulkanDescriptorSetLayout* vulkanDescriptorSetLayout);
	VulkanPipeline* CreateVulkanPipeline(PipelineCI& pipelineCI);
	VulkanRenderPass* CreateVulkanRenderPass();

	// Uniform Buffer
	void UpdateUniformBuffer(void* data, uint32_t size);

	// Frame Resource
	VulkanFramebuffer* CreateFramebuffer(VulkanRenderPass* vulkanRenderPass);
	VulkanCommandBuffer* GetCurrCommandBuffer();
	VulkanBuffer* GetCurrUniformBuffer();

private:

	//

private:

	VulkanInstance* m_VulkanInstance;
	VulkanSurface* m_VulkanSurface;
	VulkanDevice* m_VulkanDevice;
	VulkanSwapChain* m_VulkanSwapChain;
	VulkanCommandPool* m_VulkanCommandPool;

	size_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	VulkanBuffer* m_StagingBuffer;

	std::vector<VulkanBuffer*> m_UniformBuffers;

	VkFormat m_DepthFormat;
	VulkanImage* m_DepthImage;
};

void CreateVulkanDriver();
VulkanDriver& GetVulkanDriver();
void ReleaseVulkanDriver();