#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"
#include <vector>

class VKCommandBuffer : public NonCopyable
{
public:

	VKCommandBuffer(VkDevice vkDevice, VkCommandPool vkCommandPool, VkCommandBufferLevel level);
	virtual ~VKCommandBuffer();

	void Begin();
	void End();

	void BeginRenderPass(VkRenderPass vkRenderPass, VkFramebuffer vkFrameBuffer, VkRect2D& area, std::vector<VkClearValue>& clearValues);
	void NextSubpass();
	void EndRenderPass();

	void SetViewport(VkViewport& viewport);
	void SetScissor(VkRect2D& area);

	void CopyBuffer(VkBuffer src, VkBuffer dst, VkBufferCopy& region);
	void CopyBufferToImage(VkBuffer src, VkImage dst, uint32_t width, uint32_t height, const std::vector<std::vector<std::vector<uint64_t>>>& offsets);

	void ImageMemoryBarrier(VkImage image, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount);

	void BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, uint32_t index, VkDescriptorSet set);
	void BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, uint32_t index, VkDescriptorSet set, const std::vector<uint32_t>& offsets);
	void BindPipeline(VkPipelineBindPoint bindPoint, VkPipeline vkPipeline);
	
	void BindVertexBuffer(uint32_t bind, VkBuffer vkBuffer);
	void BindIndexBuffer(VkBuffer vkBuffer, VkIndexType indexType);
	
	void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

	void PushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags pcStage, uint32_t offset, uint32_t size, void* data);

	// todo:temp
	bool m_NeedSubmit = false;

	//// TimeStamp
	//void WriteTimeStamp(std::string name);
	

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
};