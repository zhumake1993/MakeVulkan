#include "VKCommandBuffer.h"
#include "VulkanTools.h"
#include "DeviceProperties.h"

VKCommandBuffer::VKCommandBuffer(VkDevice vkDevice, VkCommandPool vkCommandPool, VkCommandBufferLevel level) :
	device(vkDevice),
	commandPool(vkCommandPool)
{
	VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.pNext = nullptr;
	cmdBufferAllocInfo.commandPool = vkCommandPool;
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

void VKCommandBuffer::BeginRenderPass(VkRenderPass vkRenderPass, VkFramebuffer vkFrameBuffer, VkRect2D& area, std::vector<VkClearValue>& clearValues)
{
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vkRenderPass;
	renderPassBeginInfo.framebuffer = vkFrameBuffer;
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

void VKCommandBuffer::CopyBuffer(VkBuffer src, VkBuffer dst, VkBufferCopy & region)
{
	vkCmdCopyBuffer(commandBuffer, src, dst, 1, &region);
}

void VKCommandBuffer::CopyBufferToImage(VkBuffer src, VkImage dst, uint32_t width, uint32_t height, const std::vector<std::vector<std::vector<uint64_t>>>& offsets)
{
	size_t faceCount = offsets.size();
	ASSERT(faceCount == 1 || faceCount == 6, "wrong faceCount.");

	std::vector<VkBufferImageCopy> bufferCopyRegions;
	for (uint32_t face = 0; face < offsets.size(); face++)
	{
		for (uint32_t layer = 0; layer < offsets[face].size(); layer++)
		{
			for (uint32_t level = 0; level < offsets[face][layer].size(); level++)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = layer * 6 + face;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageOffset = { 0,0,0 };
				bufferCopyRegion.imageExtent.width = width >> level;
				bufferCopyRegion.imageExtent.height = height >> level;
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offsets[face][layer][level];
				bufferCopyRegion.bufferRowLength = 0;
				bufferCopyRegion.bufferImageHeight = 0;
				bufferCopyRegions.push_back(bufferCopyRegion);
			}
		}
	}

	vkCmdCopyBufferToImage(commandBuffer, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());
}

void VKCommandBuffer::ImageMemoryBarrier(VkImage image, VkPipelineStageFlags srcPSF, VkPipelineStageFlags dstPSF, VkAccessFlags srcAF, VkAccessFlags dstAF, VkImageLayout oldIL, VkImageLayout newIL, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount)
{
	VkImageSubresourceRange imageSubresourceRange = {};
	imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageSubresourceRange.baseMipLevel = 0;
	imageSubresourceRange.levelCount = mipLevels;
	imageSubresourceRange.baseArrayLayer = 0;
	imageSubresourceRange.layerCount = faceCount * layerCount;

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = srcAF;
	imageMemoryBarrier.dstAccessMask = dstAF;
	imageMemoryBarrier.oldLayout = oldIL;
	imageMemoryBarrier.newLayout = newIL;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = imageSubresourceRange;

	vkCmdPipelineBarrier(commandBuffer, srcPSF, dstPSF, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VKCommandBuffer::BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, uint32_t index, VkDescriptorSet set)
{
	BindDescriptorSet(bindPoint, pipelineLayout, index, set, {});
}

void VKCommandBuffer::BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, uint32_t index, VkDescriptorSet set, const std::vector<uint32_t>& offsets)
{
	// Each element of pDynamicOffsets which corresponds to a descriptor binding with type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
	// must be a multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment

	if (offsets.size() > 0)
	{
		auto& dp = GetDeviceProperties();

		for (auto offset : offsets)
		{
			if (offset % dp.deviceProperties.limits.minUniformBufferOffsetAlignment != 0)
			{
				LOGE("offset must be a multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment");
			}
		}
	}

	vkCmdBindDescriptorSets(commandBuffer, bindPoint, pipelineLayout, index, 1, &set, static_cast<uint32_t>(offsets.size()), offsets.data());
}

void VKCommandBuffer::BindPipeline(VkPipelineBindPoint bindPoint, VkPipeline vkPipeline)
{
	vkCmdBindPipeline(commandBuffer, bindPoint, vkPipeline);
}

void VKCommandBuffer::BindVertexBuffer(uint32_t bind, VkBuffer vkBuffer)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, bind, 1, &vkBuffer, &offset);
}

void VKCommandBuffer::BindIndexBuffer(VkBuffer vkBuffer, VkIndexType indexType)
{
	vkCmdBindIndexBuffer(commandBuffer, vkBuffer, 0, indexType);
}

void VKCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VKCommandBuffer::PushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags pcStage, uint32_t offset, uint32_t size, void * data)
{
	vkCmdPushConstants(commandBuffer, pipelineLayout, pcStage, offset, size, data);
}