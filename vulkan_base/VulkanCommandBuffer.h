#pragma once

#include "VulkanCommon.h"

class VulkanDevice;
class VulkanCommandPool;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanPipeline;
class VulkanBuffer;
class VulkanPipelineLayout;
class VulkanDescriptorSet;
class VulkanImage;

class VulkanCommandBuffer
{

public:

	VulkanCommandBuffer(VulkanDevice* vulkanDevice, VulkanCommandPool* vulkanCommandPool, VkCommandBufferLevel level);
	~VulkanCommandBuffer();

	void CleanUp();

	void Begin();
	void End();
	void BeginRenderPass(VulkanRenderPass *vulkanRenderPass, VulkanFramebuffer* vulkanFrameBuffer, VkRect2D& area, VkClearValue& clearValue);
	void SetViewport(VkViewport& viewport);
	void SetScissor(VkRect2D& area);
	void BindPipeline(VkPipelineBindPoint bindPoint, VulkanPipeline* vulkanPipeline);
	void BindVertexBuffer(uint32_t bind, VulkanBuffer* vulkanBuffer);
	void BindIndexBuffer(VulkanBuffer* vulkanBuffer, VkIndexType indexType);
	void BindDescriptorSet(VkPipelineBindPoint bindPoint, VulkanPipelineLayout* vulkanPipelineLayout, VulkanDescriptorSet* vulkanDescriptorSet);
	void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
	void EndRenderPass();
	void CopyBuffer(VulkanBuffer* src, VulkanBuffer* dst, VkBufferCopy& region);
	void CopyBufferToImage(VulkanBuffer* src, VulkanImage* dst);
	void ImageMemoryBarrier(VulkanImage* img, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL);

private:

	//

public:

	VkCommandBufferLevel m_Level;
	VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

private:

	VulkanDevice* m_VulkanDevice = nullptr;
	VulkanCommandPool* m_VulkanCommandPool = nullptr;
};