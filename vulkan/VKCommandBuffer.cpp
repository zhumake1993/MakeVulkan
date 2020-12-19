#include "VKCommandBuffer.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"
#include "VKCommandPool.h"

#include "VKBuffer.h"
#include "VKImage.h"

#include "VKFramebuffer.h"
#include "VKRenderPass.h"

#include "VKPipeline.h"

#include "VulkanDriver.h"
#include "GPUProfilerMgr.h"

VKCommandBuffer::VKCommandBuffer(VKDevice * vkDevice, VKCommandPool * vkCommandPool, VkCommandBufferLevel level) :
	device(vkDevice->device),
	commandPool(vkCommandPool->commandPool)
{
	VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.pNext = nullptr;
	cmdBufferAllocInfo.commandPool = vkCommandPool->commandPool;
	cmdBufferAllocInfo.level = level;
	cmdBufferAllocInfo.commandBufferCount = 1;

	VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cmdBufferAllocInfo, &commandBuffer));
}

VKCommandBuffer::~VKCommandBuffer()
{
	if (device != VK_NULL_HANDLE && commandPool != VK_NULL_HANDLE && commandBuffer != VK_NULL_HANDLE) {
		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
		commandBuffer = VK_NULL_HANDLE;
	}
}

void VKCommandBuffer::Begin()
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
	// If the command buffer was already recorded once, then a call to vkBeginCommandBuffer will implicitly reset it.
	VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
}

void VKCommandBuffer::End()
{
	VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));
}

void VKCommandBuffer::CopyVKBuffer(VKBuffer * src, VKBuffer * dst, VkBufferCopy & region)
{
	vkCmdCopyBuffer(commandBuffer, src->buffer, dst->buffer, 1, &region);
}

void VKCommandBuffer::CopyVKBufferToVKImage(VKBuffer * src, VKImage * dst)
{
	VkBufferImageCopy bufferImageCopyInfo = {};
	bufferImageCopyInfo.bufferOffset = 0;
	bufferImageCopyInfo.bufferRowLength = 0;
	bufferImageCopyInfo.bufferImageHeight = 0;
	bufferImageCopyInfo.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT,0,0,1 };
	bufferImageCopyInfo.imageOffset = { 0,0,0 };
	bufferImageCopyInfo.imageExtent = { dst->width,dst->height,1 };

	vkCmdCopyBufferToImage(commandBuffer, src->buffer, dst->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyInfo);
}

void VKCommandBuffer::ImageMemoryBarrier(VKImage * image, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL)
{
	VkImageSubresourceRange imageSubresourceRange = {};
	imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageSubresourceRange.baseMipLevel = 0;
	imageSubresourceRange.levelCount = 1;;
	imageSubresourceRange.baseArrayLayer = 0;
	imageSubresourceRange.layerCount = 1;

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = srcAF;
	imageMemoryBarrier.dstAccessMask = dstAF;
	imageMemoryBarrier.oldLayout = oldIL;
	imageMemoryBarrier.newLayout = newIL;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image->image;
	imageMemoryBarrier.subresourceRange = imageSubresourceRange;

	vkCmdPipelineBarrier(commandBuffer, srcPSF, dstPSF, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VKCommandBuffer::BeginRenderPass(VKRenderPass * vkRenderPass, VKFramebuffer * vkFrameBuffer, VkRect2D & area, std::vector<VkClearValue>& clearValues)
{
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vkRenderPass->renderPass;
	renderPassBeginInfo.framebuffer = vkFrameBuffer->framebuffer;
	renderPassBeginInfo.renderArea = area;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VKCommandBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(commandBuffer);
}

void VKCommandBuffer::SetViewport(VkViewport & viewport)
{
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void VKCommandBuffer::SetScissor(VkRect2D & area)
{
	vkCmdSetScissor(commandBuffer, 0, 1, &area);
}

void VKCommandBuffer::BindPipeline(VkPipelineBindPoint bindPoint, VKPipeline * vkPipeline)
{
	vkCmdBindPipeline(commandBuffer, bindPoint, vkPipeline->pipeline);
}

void VKCommandBuffer::BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, uint32_t index, VkDescriptorSet set, uint32_t offset)
{
	// Each element of pDynamicOffsets which corresponds to a descriptor binding with type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
	// must be a multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment
	vkCmdBindDescriptorSets(commandBuffer, bindPoint, pipelineLayout, index, 1, &set, offset == -1 ? 0 : 1, &offset);
}

void VKCommandBuffer::PushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags pcStage, uint32_t offset, uint32_t size, void * data)
{
	vkCmdPushConstants(commandBuffer, pipelineLayout, pcStage, offset, size, data);
}

void VKCommandBuffer::BindVertexBuffer(uint32_t bind, VKBuffer * vkBuffer)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, bind, 1, &vkBuffer->buffer, &offset);
}

void VKCommandBuffer::BindIndexBuffer(VKBuffer * vkBuffer, VkIndexType indexType)
{
	vkCmdBindIndexBuffer(commandBuffer, vkBuffer->buffer, 0, indexType);
}

void VKCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VKCommandBuffer::WriteTimeStamp(std::string name)
{
	GetVulkanDriver().GetGPUProfilerMgr()->WriteTimeStamp(this, name);
}