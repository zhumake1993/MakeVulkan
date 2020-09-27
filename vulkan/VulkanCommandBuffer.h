#pragma once

#include "Common.h"
#include "VulkanFwd.h"

class VulkanCommandBuffer
{

public:

	VulkanCommandBuffer(VulkanDevice* vulkanDevice, VulkanCommandPool* vulkanCommandPool, VkCommandBufferLevel level);
	~VulkanCommandBuffer();

	void Begin();
	void End();

	// Resource
	void CopyBuffer(VulkanBuffer* src, VulkanBuffer* dst, VkBufferCopy& region);
	void CopyBufferToImage(VulkanBuffer* src, VKImage* dst);
	void ImageMemoryBarrier(VKImage* image, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL);
	void UploadVulkanBuffer(VulkanBuffer* vertexBuffer, void* data, VkDeviceSize size, VulkanBuffer* stagingBuffer);
	void UploadVKImage(VKImage* image, void* data, VkDeviceSize size, VulkanBuffer* stagingBuffer);

	//
	void BeginRenderPass(VulkanRenderPass *vulkanRenderPass, VulkanFramebuffer* vulkanFrameBuffer, VkRect2D& area, std::vector<VkClearValue>& clearValues);
	void SetViewport(VkViewport& viewport);
	void SetScissor(VkRect2D& area);
	void BindPipeline(VkPipelineBindPoint bindPoint, VulkanPipeline* vulkanPipeline);
	void BindVertexBuffer(uint32_t bind, VulkanBuffer* vulkanBuffer);
	void BindIndexBuffer(VulkanBuffer* vulkanBuffer, VkIndexType indexType);
	void BindDescriptorSet(VkPipelineBindPoint bindPoint, VKPipelineLayout* pipelineLayout, VkDescriptorSet set, uint32_t offset = -1);
	void PushConstants(VKPipelineLayout* pipelineLayout, VkShaderStageFlags pcStage, uint32_t offset, uint32_t size, void* data);
	void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
	void EndRenderPass();

private:

	//

public:

	VkCommandBufferLevel m_Level;
	VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
	VulkanCommandPool* m_VulkanCommandPool = nullptr;
};