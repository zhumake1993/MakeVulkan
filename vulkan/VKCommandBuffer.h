#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"

struct VKCommandBuffer : public NonCopyable
{
	VKCommandBuffer(VKDevice* vkDevice, VKCommandPool* vkCommandPool, VkCommandBufferLevel level);
	~VKCommandBuffer();

	// Begin End

	void Begin();
	void End();

	// Resource

	void CopyVKBuffer(VKBuffer* src, VKBuffer* dst, VkBufferCopy& region);
	void CopyVKBufferToVKImage(VKBuffer* src, VKImage* dst);
	
	void ImageMemoryBarrier(VKImage* image, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL);
	
	// Render Pass

	void BeginRenderPass(VKRenderPass *vkRenderPass, VKFramebuffer* vkFrameBuffer, VkRect2D& area, std::vector<VkClearValue>& clearValues);
	void EndRenderPass();

	// Render Status

	void SetViewport(VkViewport& viewport);
	void SetScissor(VkRect2D& area);
	void BindPipeline(VkPipelineBindPoint bindPoint, VKPipeline* vkPipeline);
	void BindDescriptorSet(VkPipelineBindPoint bindPoint, VKPipelineLayout* pipelineLayout, VkDescriptorSet set, uint32_t offset = -1);
	void PushConstants(VKPipelineLayout* pipelineLayout, VkShaderStageFlags pcStage, uint32_t offset, uint32_t size, void* data);

	void BindVertexBuffer(uint32_t bind, VKBuffer* vkBuffer);
	void BindIndexBuffer(VKBuffer* vkBuffer, VkIndexType indexType);
	
	void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
	

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
};