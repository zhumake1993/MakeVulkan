#pragma once

#include "Application.h"

#include "Camera.h"

class VulkanInstance;
class VulkanSurface;
class VulkanDevice;
class VulkanSwapChain;
class VulkanCommandPool;
class VulkanCommandBuffer;
class VulkanSemaphore;
class VulkanFence;
class VulkanFramebuffer;
class VulkanRenderPass;
class VulkanPipelineLayout;
class VulkanPipeline;
class VulkanBuffer;
class VulkanImage;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanDescriptorSet;

struct UniformBuffer
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class VulkanBase : public Application
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
	VulkanBase();
	virtual ~VulkanBase();

	void CleanUp() override;
	void Init() override;
	virtual void Prepare() override{}

protected:

	void UploadBuffer(VulkanBuffer* vertexBuffer, void* data, uint32_t size);
	void UploadImage(VulkanImage* vulkanImage, void* data, uint32_t size);

private:

	void Draw() override;
	virtual void Logic(float deltaTime) {}
	virtual void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer, VulkanFramebuffer* vulkanFramebuffer) {}
	void CreateFramebuffer(VulkanFramebuffer* vulkanFramebuffer, VkImageView& imageView);

protected:

	VulkanInstance* m_VulkanInstance;
	VulkanSurface* m_VulkanSurface;
	VulkanDevice* m_VulkanDevice;
	VulkanSwapChain* m_VulkanSwapChain;

	VulkanCommandPool* m_VulkanCommandPool;

	size_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	VulkanRenderPass* m_VulkanRenderPass;

	VulkanBuffer* m_StagingBuffer;

	// ÉãÏñ»ú
	Camera m_Camera;
	std::vector<VulkanBuffer*> m_UniformBuffers;

	// stats
	uint32_t m_FrameIndex = 0;
	uint32_t m_AccumulateCounter = 0;
	float m_AccumulateTime = 0;
	float m_FPS = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp = std::chrono::high_resolution_clock::now();
};