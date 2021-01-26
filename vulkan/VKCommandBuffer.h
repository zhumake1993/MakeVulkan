#pragma once

#include "Env.h"
#include "NonCopyable.h"

struct VKCommandBuffer : public NonCopyable
{
	VKCommandBuffer(VkDevice vkDevice, VkCommandPool vkCommandPool, VkCommandBufferLevel level);
	~VKCommandBuffer();

	void Begin();
	void End();

	void BeginRenderPass(VkRenderPass vkRenderPass, VkFramebuffer vkFrameBuffer, VkRect2D& area, std::vector<VkClearValue>& clearValues);
	void EndRenderPass();

	void SetViewport(VkViewport& viewport);
	void SetScissor(VkRect2D& area);

	//// Resource

	//void CopyVKBuffer(VKBuffer* src, VKBuffer* dst, VkBufferCopy& region);
	//void CopyVKBufferToVKImage(VKBuffer* src, VKImage* dst);
	//
	//void ImageMemoryBarrier(VKImage* image, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL);
	//
	//// Render Pass

	

	//// Render Status

	
	//void BindPipeline(VkPipelineBindPoint bindPoint, VKPipeline* vkPipeline);
	//void BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, uint32_t index, VkDescriptorSet set, uint32_t offset = -1);
	//void PushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags pcStage, uint32_t offset, uint32_t size, void* data);

	//void BindVertexBuffer(uint32_t bind, VKBuffer* vkBuffer);
	//void BindIndexBuffer(VKBuffer* vkBuffer, VkIndexType indexType);
	//
	//void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

	//// TimeStamp
	//void WriteTimeStamp(std::string name);
	

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
};