#pragma once

#include "Application.h"

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

struct VertexData {
	float   x, y, z, w;
	float   r, g, b, a;
};

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

class VulkanBase : public Application
{

public:
	VulkanBase();
	virtual ~VulkanBase();

	virtual void CleanUp() override;
	virtual void Init() override;
	virtual void Prepare() override;

protected:

	//

private:

	virtual void CreatePipeline(std::shared_ptr<VulkanPipelineLayout> vulkanPipelineLayout);
	virtual void PrepareVertices();

	virtual void Draw() override;
	void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer, VulkanFramebuffer* vulkanFramebuffer);
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
	VulkanPipeline* m_VulkanPipeline;

	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;
};