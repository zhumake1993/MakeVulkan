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
class VulkanBuffer;
class VulkanImage;

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

	virtual void CreatePipeline();
	virtual void PrepareVertices();
	virtual void PrepareTextures();
	virtual void PrepareUniformBuffer();

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
	std::shared_ptr<VulkanPipelineLayout> m_VulkanPipelineLayout;

	VulkanBuffer* m_VertexBuffer;
	VulkanBuffer* m_IndexBuffer;
	VulkanBuffer* m_VertexStagingBuffer;
	VulkanBuffer* m_IndexStagingBuffer;

	VulkanImage* m_Image;
	VulkanBuffer* m_ImageStagingBuffer;

	VulkanBuffer* m_UniformBuffer;
	VulkanBuffer* m_UniformStagingBuffer;

	VkDescriptorPool m_DescriptorPool;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkDescriptorSet m_DescriptorSet;
};