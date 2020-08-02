#pragma once

#include "Application.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPipeline.h"
#include <string>
#include <iostream>

struct VertexData {
	float   x, y, z, w;
	float   r, g, b, a;
};

struct FrameResource {
	VkFramebuffer framebuffer;
	VulkanCommandBuffer vulkanCommandBuffer;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore finishedRenderingSemaphore;
	VkFence fence;

	FrameResource() :
		framebuffer(VK_NULL_HANDLE),
		vulkanCommandBuffer(),
		imageAvailableSemaphore(VK_NULL_HANDLE),
		finishedRenderingSemaphore(VK_NULL_HANDLE),
		fence(VK_NULL_HANDLE) {
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