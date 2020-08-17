#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanPipelineLayout.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"
#include "Tools.h"

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* vulkanDevice, VulkanCommandPool* vulkanCommandPool, VkCommandBufferLevel level):
	m_VulkanDevice(vulkanDevice),
	m_VulkanCommandPool(vulkanCommandPool),
	m_Level(level)
{
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

void VulkanCommandBuffer::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE
		&& m_VulkanCommandPool && m_VulkanCommandPool->m_CommandPool != VK_NULL_HANDLE
		&& m_CommandBuffer != VK_NULL_HANDLE) {
		vkFreeCommandBuffers(m_VulkanDevice->m_LogicalDevice, m_VulkanCommandPool->m_CommandPool, 1, &m_CommandBuffer);
		m_CommandBuffer = VK_NULL_HANDLE;
	}
}

void VulkanCommandBuffer::Begin()
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
	VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo));
}

void VulkanCommandBuffer::End()
{
	VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer));;
}

void VulkanCommandBuffer::BeginRenderPass(VulkanRenderPass *vulkanRenderPass, VulkanFramebuffer* vulkanFramebuffer, VkRect2D& area, VkClearValue& clearValue)
{
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vulkanRenderPass->m_RenderPass;
	renderPassBeginInfo.framebuffer = vulkanFramebuffer->m_Framebuffer;
	renderPassBeginInfo.renderArea = area;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValue;
	vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::SetViewport(VkViewport & viewport)
{
	vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
}

void VulkanCommandBuffer::SetScissor(VkRect2D & area)
{
	vkCmdSetScissor(m_CommandBuffer, 0, 1, &area);
}

void VulkanCommandBuffer::BindPipeline(VkPipelineBindPoint bindPoint, VulkanPipeline * vulkanPipeline)
{
	vkCmdBindPipeline(m_CommandBuffer, bindPoint, vulkanPipeline->m_Pipeline);
}

void VulkanCommandBuffer::BindVertexBuffer(uint32_t bind, VulkanBuffer * vulkanBuffer)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(m_CommandBuffer, bind, 1, &vulkanBuffer->m_Buffer, &offset);
}

void VulkanCommandBuffer::BindIndexBuffer(VulkanBuffer * vulkanBuffer, VkIndexType indexType)
{
	vkCmdBindIndexBuffer(m_CommandBuffer, vulkanBuffer->m_Buffer, 0, indexType);
}

void VulkanCommandBuffer::BindDescriptorSet(VkPipelineBindPoint bindPoint, VulkanPipelineLayout * vulkanPipelineLayout, VulkanDescriptorSet * vulkanDescriptorSet)
{
	vkCmdBindDescriptorSets(m_CommandBuffer, bindPoint, vulkanPipelineLayout->m_PipelineLayout, 0, 1, &vulkanDescriptorSet->m_DescriptorSet, 0, nullptr);
}

void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(m_CommandBuffer);
}

void VulkanCommandBuffer::CopyBuffer(VulkanBuffer * src, VulkanBuffer * dst, VkBufferCopy & region)
{
	vkCmdCopyBuffer(m_CommandBuffer, src->m_Buffer, dst->m_Buffer, 1, &region);
}

void VulkanCommandBuffer::CopyBufferToImage(VulkanBuffer * src, VulkanImage * dst)
{
	VkBufferImageCopy bufferImageCopyInfo = {};
	bufferImageCopyInfo.bufferOffset = 0;
	bufferImageCopyInfo.bufferRowLength = 0;
	bufferImageCopyInfo.bufferImageHeight = 0;
	bufferImageCopyInfo.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT,0,0,1 };
	bufferImageCopyInfo.imageOffset = { 0,0,0 };
	bufferImageCopyInfo.imageExtent = { dst->m_Width,dst->m_Height,1 };

	vkCmdCopyBufferToImage(m_CommandBuffer, src->m_Buffer, dst->m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyInfo);
}

void VulkanCommandBuffer::ImageMemoryBarrier(VulkanImage * img, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL)
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
	imageMemoryBarrier.image = img->m_Image;
	imageMemoryBarrier.subresourceRange = imageSubresourceRange;

	vkCmdPipelineBarrier(m_CommandBuffer, srcPSF, dstPSF, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}
