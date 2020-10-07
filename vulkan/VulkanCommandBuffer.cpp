#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VKPipelineLayout.h"
#include "VKImage.h"
#include "Tools.h"

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* vulkanDevice, VulkanCommandPool* vulkanCommandPool, VkCommandBufferLevel level):
	m_VulkanDevice(vulkanDevice),
	m_VulkanCommandPool(vulkanCommandPool),
	m_Level(level)
{
}

VulkanCommandBuffer::~VulkanCommandBuffer()
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
	// If the command buffer was already recorded once, then a call to vkBeginCommandBuffer will implicitly reset it.
	VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo));
}

void VulkanCommandBuffer::End()
{
	VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer));;
}

void VulkanCommandBuffer::CopyBuffer(VulkanBuffer * src, VulkanBuffer * dst, VkBufferCopy & region)
{
	vkCmdCopyBuffer(m_CommandBuffer, src->m_Buffer, dst->m_Buffer, 1, &region);
}

void VulkanCommandBuffer::CopyBufferToImage(VulkanBuffer * src, VKImage * dst)
{
	VkBufferImageCopy bufferImageCopyInfo = {};
	bufferImageCopyInfo.bufferOffset = 0;
	bufferImageCopyInfo.bufferRowLength = 0;
	bufferImageCopyInfo.bufferImageHeight = 0;
	bufferImageCopyInfo.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT,0,0,1 };
	bufferImageCopyInfo.imageOffset = { 0,0,0 };
	bufferImageCopyInfo.imageExtent = { dst->GetWidth(),dst->GetHeight(),1 };

	vkCmdCopyBufferToImage(m_CommandBuffer, src->m_Buffer, dst->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyInfo);
}

void VulkanCommandBuffer::ImageMemoryBarrier(VKImage * image, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL)
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
	imageMemoryBarrier.image = image->GetImage();
	imageMemoryBarrier.subresourceRange = imageSubresourceRange;

	vkCmdPipelineBarrier(m_CommandBuffer, srcPSF, dstPSF, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VulkanCommandBuffer::UploadVulkanBuffer(VulkanBuffer * vertexBuffer, void * data, VkDeviceSize size, VulkanBuffer * stagingBuffer)
{
	stagingBuffer->Copy(data, 0, size);

	Begin();

	VkBufferCopy bufferCopyInfo = {};
	bufferCopyInfo.srcOffset = 0;
	bufferCopyInfo.dstOffset = 0;
	bufferCopyInfo.size = size;
	CopyBuffer(stagingBuffer, vertexBuffer, bufferCopyInfo);

	// 经测试发现没有这一步也没问题（许多教程也的确没有这一步）
	// 个人认为是因为调用了WaitIdle
	//vkCmdPipelineBarrier

	End();

	m_VulkanDevice->Submit(this);

	m_VulkanDevice->WaitIdle();
}

void VulkanCommandBuffer::UploadVKImage(VKImage* image, void * data, VkDeviceSize size, VulkanBuffer * stagingBuffer)
{
	stagingBuffer->Copy(data, 0, size);

	Begin();

	ImageMemoryBarrier(image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	CopyBufferToImage(stagingBuffer, image);

	ImageMemoryBarrier(image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	End();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	VK_CHECK_RESULT(vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE));

	//m_VulkanDevice->Submit(this);

	m_VulkanDevice->WaitIdle();
}

void VulkanCommandBuffer::BeginRenderPass(VulkanRenderPass *vulkanRenderPass, VulkanFramebuffer* vulkanFramebuffer, VkRect2D& area, std::vector<VkClearValue>& clearValues)
{
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vulkanRenderPass->m_RenderPass;
	renderPassBeginInfo.framebuffer = vulkanFramebuffer->m_Framebuffer;
	renderPassBeginInfo.renderArea = area;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();
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

void VulkanCommandBuffer::BindDescriptorSet(VkPipelineBindPoint bindPoint, VKPipelineLayout* pipelineLayout, VkDescriptorSet set, uint32_t offset)
{
	vkCmdBindDescriptorSets(m_CommandBuffer, bindPoint, pipelineLayout->GetLayout(), 0, 1, &set, offset == -1 ? 0 : 1, &offset);
}

void VulkanCommandBuffer::PushConstants(VKPipelineLayout * pipelineLayout, VkShaderStageFlags pcStage, uint32_t offset, uint32_t size, void * data)
{
	vkCmdPushConstants(m_CommandBuffer, pipelineLayout->GetLayout(), pcStage, offset, size, data);
}

void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(m_CommandBuffer);
}