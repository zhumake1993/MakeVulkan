#pragma once

#include "Application.h"




//#include "VulkanPipeline.h"

class VulkanInstance;
class VulkanSurface;
class VulkanDevice;
class VulkanSwapChain;
class VulkanCommandPool;
class VulkanCommandBuffer;
class VulkanSemaphore;
class VulkanFence;

struct VertexData {
	float   x, y, z, w;
	float   r, g, b, a;
};

struct FrameResource {
	VkFramebuffer framebuffer;
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

	virtual void SetupRenderPass();
	virtual void CreatePipeline();
	virtual void PrepareVertices();

	virtual void Draw() override;
	void RecordCommandBuffer(VulkanCommandBuffer& vulkanCommandBuffer, VkFramebuffer& framebuffer);
	void CreateFramebuffer(VkFramebuffer& framebuffer, VkImageView& imageView);

protected:

	VulkanInstance* m_VulkanInstance;
	VulkanSurface* m_VulkanSurface;
	VulkanDevice* m_VulkanDevice;
	VulkanSwapChain* m_VulkanSwapChain;

	VulkanCommandPool* m_VulkanCommandPool;

	size_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	VkRenderPass m_RenderPass;
	std::vector<VkFramebuffer> m_FrameBuffers;
	VkPipelineLayout m_PipelineLayout;
	VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
	VkPipeline m_Pipeline;

	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;
};