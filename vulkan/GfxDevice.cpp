#include "GfxDevice.h"
#include "Tools.h"
#include "DeviceProperties.h"
#include "Settings.h"
#include "VulkanTools.h"

#include "VKInstance.h"
#include "VKSurface.h"
#include "VKDevice.h"
#include "VKSwapChain.h"
#include "VKCommandPool.h"

#include "VKCommandBuffer.h"
#include "VKRenderPass.h"

#include "VKImage.h"

#include "BufferManager.h"
#include "DescriptorSetManager.h"
#include "PipelineManager.h"

#include "VKGpuProgram.h"

#include "Shader.h"

GfxDevice* gfxDevice;

void CreateGfxDevice()
{
	gfxDevice = new GfxDevice();
}

GfxDevice& GetGfxDevice()
{
	return *gfxDevice;
}

void ReleaseGfxDevice()
{
	RELEASE(gfxDevice);
}

GfxDevice::GfxDevice()
{
	auto& dp = GetDeviceProperties();

	m_VKInstance = new VKInstance();
	m_VKSurface = new VKSurface(m_VKInstance->instance);
	m_VKDevice = new VKDevice(m_VKInstance->instance, m_VKSurface->surface);
	m_VKSwapChain = new VKSwapChain(m_VKDevice->physicalDevice, m_VKDevice->device, m_VKSurface->surface);
	m_VKCommandPool = new VKCommandPool(m_VKDevice->device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, dp.selectedQueueFamilyIndex);;

	//todo
	dp.Log();

	// depth
	m_DepthFormat = GetSupportedDepthFormat();
	m_DepthImage = new VKImage(kImageType2D, m_VKDevice->device, VK_IMAGE_TYPE_2D, m_DepthFormat, windowWidth, windowHeight, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT);

	m_VKRenderPass = new VKRenderPass(m_VKDevice->device, m_VKSwapChain->format.format, m_DepthFormat);

	m_FrameResources.resize(FrameResourcesCount);
	for (size_t i = 0; i < FrameResourcesCount; ++i)
	{
		m_FrameResources[i].commandBuffer = new VKCommandBuffer(m_VKDevice->device, m_VKCommandPool->commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		m_FrameResources[i].imageAvailableSemaphore = CreateVKSemaphore();
		m_FrameResources[i].finishedRenderingSemaphore = CreateVKSemaphore();
		m_FrameResources[i].fence = CreateVKFence(true);
	}

	m_Framebuffers.resize(m_VKSwapChain->numberOfImages);
	for (size_t i = 0; i < m_VKSwapChain->numberOfImages; ++i)
	{
		m_Framebuffers[i] = CreateVkFramebuffer(m_VKRenderPass->renderPass, m_VKSwapChain->swapChainImageViews[i], m_DepthImage->m_View, windowWidth, windowHeight);
	}

	m_UploadCommandBuffer = new VKCommandBuffer(m_VKDevice->device, m_VKCommandPool->commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	m_BufferManager = new BufferManager(m_VKDevice->device);

	m_DescriptorSetManager = new DescriptorSetManager(m_VKDevice->device);

	m_PipelineManager = new PipelineManager(m_VKDevice->device);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { m_DescriptorSetManager->GetDSLGlobal(),m_DescriptorSetManager->GetDSLPerView() };
	m_PipelineManager->SetDummyPipelineLayout(m_PipelineManager->CreatePipelineLayout(descriptorSetLayouts));
}

GfxDevice::~GfxDevice()
{
	RELEASE(m_BufferManager);
	RELEASE(m_DescriptorSetManager);
	RELEASE(m_PipelineManager);

	RELEASE(m_UploadCommandBuffer);

	for (size_t i = 0; i < m_VKSwapChain->numberOfImages; ++i)
	{
		vkDestroyFramebuffer(m_VKDevice->device, m_Framebuffers[i], nullptr);
		m_Framebuffers[i] = VK_NULL_HANDLE;
	}

	for (size_t i = 0; i < FrameResourcesCount; ++i)
	{
		RELEASE(m_FrameResources[i].commandBuffer);

		vkDestroySemaphore(m_VKDevice->device, m_FrameResources[i].imageAvailableSemaphore, nullptr);
		m_FrameResources[i].imageAvailableSemaphore = VK_NULL_HANDLE;

		vkDestroySemaphore(m_VKDevice->device, m_FrameResources[i].finishedRenderingSemaphore, nullptr);
		m_FrameResources[i].finishedRenderingSemaphore = VK_NULL_HANDLE;

		vkDestroyFence(m_VKDevice->device, m_FrameResources[i].fence, nullptr);
		m_FrameResources[i].fence = VK_NULL_HANDLE;
	}

	RELEASE(m_VKRenderPass);
	RELEASE(m_DepthImage);

	RELEASE(m_VKCommandPool);
	RELEASE(m_VKSwapChain);
	RELEASE(m_VKDevice);
	RELEASE(m_VKSurface);
	RELEASE(m_VKInstance);
}

void GfxDevice::WaitForPresent()
{
	//PROFILER(WaitForPresent);

	auto& currFrameResource = m_FrameResources[m_FrameResourceIndex];

	VK_CHECK_RESULT(vkWaitForFences(m_VKDevice->device, 1, &currFrameResource.fence, VK_TRUE, UINT64_MAX));
	VK_CHECK_RESULT(vkResetFences(m_VKDevice->device, 1, &currFrameResource.fence));
}

void GfxDevice::AcquireNextImage()
{
	//PROFILER(AcquireNextImage);

	VkResult result = vkAcquireNextImageKHR(m_VKDevice->device, m_VKSwapChain->swapChain, UINT64_MAX, m_FrameResources[m_FrameResourceIndex].imageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);

	switch (result)
	{
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		LOG("recreate swapchain\n");
		// 不处理，直接out
		EXIT;
		break;
	default:
		LOG("Problem occurred during swap chain image acquisition!\n");
		assert(false);
	}
}

void GfxDevice::QueueSubmit()
{
	//PROFILER(Present);

	auto& currFrameResource = m_FrameResources[m_FrameResourceIndex];

	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &currFrameResource.imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &currFrameResource.commandBuffer->commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &currFrameResource.finishedRenderingSemaphore;

	VK_CHECK_RESULT(vkQueueSubmit(m_VKDevice->queue, 1, &submitInfo, currFrameResource.fence));
}

void GfxDevice::QueuePresent()
{
	//PROFILER(QueuePresent);

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_FrameResources[m_FrameResourceIndex].finishedRenderingSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_VKSwapChain->swapChain;
	presentInfo.pImageIndices = &m_ImageIndex;
	presentInfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(m_VKDevice->queue, &presentInfo);

	switch (result)
	{
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		LOG("recreate swapchain\n");
		// 不处理，直接out
		EXIT;
		break;
	default:
		LOG("Problem occurred during image presentation!\n");
		assert(false);
	}
}

void GfxDevice::Update()
{
	m_BufferManager->Update();
	m_DescriptorSetManager->Update();
	m_PipelineManager->Update();

	m_FrameIndex++;
	m_FrameResourceIndex = m_FrameIndex % FrameResourcesCount;
}

void GfxDevice::DeviceWaitIdle()
{
	vkDeviceWaitIdle(m_VKDevice->device);
}

void GfxDevice::BeginCommandBuffer()
{
	m_FrameResources[m_FrameResourceIndex].commandBuffer->Begin();
}

void GfxDevice::EndCommandBuffer()
{
	m_FrameResources[m_FrameResourceIndex].commandBuffer->End();
}

void GfxDevice::BeginRenderPass(Rect2D& renderArea, Color& clearColor, DepthStencil& clearDepthStencil)
{
	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
	clearValues[1].depthStencil = { clearDepthStencil.depth, clearDepthStencil.stencil };

	VkRect2D area = {};
	area.offset.x = renderArea.x;
	area.offset.y = renderArea.y;
	area.extent.width = renderArea.width;
	area.extent.height = renderArea.height;

	m_FrameResources[m_FrameResourceIndex].commandBuffer->BeginRenderPass(m_VKRenderPass->renderPass, m_Framebuffers[m_ImageIndex], area, clearValues);
}

void GfxDevice::EndRenderPass()
{
	m_FrameResources[m_FrameResourceIndex].commandBuffer->EndRenderPass();
}

void GfxDevice::SetViewport(Viewport & viewport)
{
	VkViewport port = {};
	port.x = viewport.x;
	port.y = viewport.y;
	port.width = static_cast<float>(viewport.width);
	port.height = static_cast<float>(viewport.height);
	port.minDepth = viewport.minDepth;
	port.maxDepth = viewport.maxDepth;

	m_FrameResources[m_FrameResourceIndex].commandBuffer->SetViewport(port);
}

void GfxDevice::SetScissor(Rect2D & scissorArea)
{
	VkRect2D area = {};
	area.offset.x = scissorArea.x;
	area.offset.y = scissorArea.y;
	area.extent.width = scissorArea.width;
	area.extent.height = scissorArea.height;

	m_FrameResources[m_FrameResourceIndex].commandBuffer->SetScissor(area);
}

Buffer * GfxDevice::CreateBuffer(BufferType bufferType, uint64_t size)
{
	switch (bufferType)
	{
		case kBufferTypeVertex:
		{
			VKBuffer* buffer = m_BufferManager->CreateBuffer(bufferType, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
			return buffer;
			break;
		}
		case kBufferTypeIndex:
		{
			VKBuffer* buffer = m_BufferManager->CreateBuffer(bufferType, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
			return buffer;
			break;
		}
		case kBufferTypeUniform:
		{
			// 上层创建的Uniform Buffer也是persistent的，例如material的buffer
			VKBuffer* buffer = m_BufferManager->CreateBuffer(bufferType, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
			return buffer;
			break;
		}
		default:
		{
			LOG("Wrong BufferType.");
			EXIT;
		}
	}
	return nullptr;
}

void GfxDevice::UpdateBuffer(Buffer * buffer, void * data, uint64_t size)
{
	BufferType bufferType = buffer->GetBufferType();

	VKBuffer* vKBuffer = static_cast<VKBuffer*>(buffer);

	switch (bufferType)
	{
		case kBufferTypeVertex:
		case kBufferTypeIndex:
		{
			m_BufferManager->UpdateBuffer(vKBuffer, data, size, m_UploadCommandBuffer);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_UploadCommandBuffer->commandBuffer;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
			VK_CHECK_RESULT(vkQueueSubmit(m_VKDevice->queue, 1, &submitInfo, VK_NULL_HANDLE));

			DeviceWaitIdle();

			break;
		}
		case kBufferTypeUniform:
		{
			m_BufferManager->UpdateBuffer(vKBuffer, data, size, nullptr);
			break;
		}
		default:
		{
			LOG("Wrong BufferType.");
			EXIT;
		}
	}
}

Image * GfxDevice::CreateImage(ImageType imageType, VkFormat format, uint32_t width, uint32_t height)
{
	switch (imageType)
	{
		case kImageType1D:
		{
			break;
		}
		case kImageType2D:
		{
			return new VKImage(imageType, m_VKDevice->device, VK_IMAGE_TYPE_2D, format, width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
				VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT);
			break;
		}
		case kImageType3D:
		{
			break;
		}
		default:
		{
			LOG("wrong ImageType.");
			EXIT;
		}
	}
	return nullptr;
}

void GfxDevice::UpdateImage(Image * image, void * data, uint64_t size)
{
	ImageType imageType = image->GetImageType();
	VKImage* vkImage = static_cast<VKImage*>(image);

	VKBuffer* stagingBuffer = m_BufferManager->GetStagingBuffer();

	if (imageType == kImageType2D)
	{
		stagingBuffer->Update(data, 0, size);

		m_UploadCommandBuffer->Begin();

		m_UploadCommandBuffer->ImageMemoryBarrier(vkImage, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		m_UploadCommandBuffer->CopyBufferToImage(stagingBuffer->GetBuffer(), vkImage);

		m_UploadCommandBuffer->ImageMemoryBarrier(vkImage, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		m_UploadCommandBuffer->End();

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_UploadCommandBuffer->commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
		VK_CHECK_RESULT(vkQueueSubmit(m_VKDevice->queue, 1, &submitInfo, VK_NULL_HANDLE));

		DeviceWaitIdle();
	}
	else
	{
		LOG("wrong ImageType.");
		EXIT;
	}
}

GpuProgram * GfxDevice::CreateGpuProgram(GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode)
{
	return new VKGpuProgram(m_VKDevice->device, parameters, vertCode, fragCode);
}

void GfxDevice::BindUniformGlobal(void * data, uint64_t size)
{
	// Create Buffer

	VKBuffer* buffer = m_BufferManager->CreateBuffer(kBufferTypeUniform, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	//VKBuffer* buffer = new VKBuffer(m_FrameIndex, m_VKDevice->device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	//m_VKGarbageCollector->AddBuffer(buffer);

	//buffer->Map();
	buffer->Update(data, 0, size);

	// Create DescriptorSet

	VkDescriptorSet descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(m_DescriptorSetManager->GetDSLGlobal());

	// Update DescriptorSet

	DescriptorInfo descriptorInfo;
	descriptorInfo.buffer.buffer = buffer->GetBuffer();
	descriptorInfo.buffer.offset = 0;
	descriptorInfo.buffer.range = size;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pBufferInfo = &descriptorInfo.buffer;

	vkUpdateDescriptorSets(m_VKDevice->device, 1, &writeDescriptorSet, 0, nullptr);

	// Bind DescriptorSet

	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineManager->GetDummyPipelineLayout(), 0, descriptorSet);
}

void GfxDevice::BindUniformPerView(void * data, uint64_t size)
{
	// Create Buffer

	VKBuffer* buffer = m_BufferManager->CreateBuffer(kBufferTypeUniform, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	//VKBuffer* buffer = new VKBuffer(m_FrameIndex, m_VKDevice->device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	//m_VKGarbageCollector->AddBuffer(buffer);

	//buffer->Map();
	buffer->Update(data, 0, size);

	// Create DescriptorSet

	VkDescriptorSet descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(m_DescriptorSetManager->GetDSLPerView());

	// Update DescriptorSet

	DescriptorInfo descriptorInfo;
	descriptorInfo.buffer.buffer = buffer->GetBuffer();
	descriptorInfo.buffer.offset = 0;
	descriptorInfo.buffer.range = size;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pBufferInfo = &descriptorInfo.buffer;

	vkUpdateDescriptorSets(m_VKDevice->device, 1, &writeDescriptorSet, 0, nullptr);

	// Bind DescriptorSet

	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineManager->GetDummyPipelineLayout(), 1, descriptorSet);
}

void GfxDevice::SetShader(Shader * shader)
{
	VKGpuProgram* gpuProgram = static_cast<VKGpuProgram*>(shader->GetGpuProgram());

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	descriptorSetLayouts.push_back(m_DescriptorSetManager->GetDSLGlobal());
	descriptorSetLayouts.push_back(m_DescriptorSetManager->GetDSLPerView());
	descriptorSetLayouts.push_back(gpuProgram->GetDSLPerMaterial());
	descriptorSetLayouts.push_back(gpuProgram->GetDSLPerDraw());

	m_PipelineManager->SetPipelineCI(descriptorSetLayouts, m_VKRenderPass->renderPass, shader->GetRenderStatus(), gpuProgram->GetVertShaderModule(), gpuProgram->GetFragShaderModule());
}

void GfxDevice::BindUniformPerDraw(Shader * shader, void * data, uint64_t size)
{
	VKGpuProgram* gpuProgram = static_cast<VKGpuProgram*>(shader->GetGpuProgram());

	// Create Buffer

	VKBuffer* buffer = m_BufferManager->CreateBuffer(kBufferTypeUniform, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false);
	//VKBuffer* buffer = new VKBuffer(m_FrameIndex, m_VKDevice->device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	//m_VKGarbageCollector->AddBuffer(buffer);

	//buffer->Map();
	buffer->Update(data, 0, size);

	// Create DescriptorSet

	VkDescriptorSet descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(gpuProgram->GetDSLPerDraw());

	// Update DescriptorSet

	DescriptorInfo descriptorInfo;
	descriptorInfo.buffer.buffer = buffer->GetBuffer();
	descriptorInfo.buffer.offset = 0;
	descriptorInfo.buffer.range = size;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.dstBinding = 0;//todo
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pBufferInfo = &descriptorInfo.buffer;

	vkUpdateDescriptorSets(m_VKDevice->device, 1, &writeDescriptorSet, 0, nullptr);

	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineManager->GetCurrPipelineLayout(), 3, descriptorSet);
}

void GfxDevice::DrawBuffer(Buffer * vertexBuffer, Buffer * indexBuffer, uint32_t indexCount, VertexDescription & vertexDescription)
{
	VkPipeline pipeline = m_PipelineManager->CreatePipeline(vertexDescription);
	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindVertexBuffer(0, static_cast<VKBuffer*>(vertexBuffer)->GetBuffer());
	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindIndexBuffer(static_cast<VKBuffer*>(indexBuffer)->GetBuffer(), VK_INDEX_TYPE_UINT32);

	m_FrameResources[m_FrameResourceIndex].commandBuffer->DrawIndexed(indexCount, 1, 0, 0, 1);
}

VkFormat GfxDevice::GetSupportedDepthFormat()
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	std::vector<VkFormat> depthFormats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	for (auto& format : depthFormats)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_VKDevice->physicalDevice, format, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			return format;
		}
	}

	LOG("Can not find supported depth format");
	return VK_FORMAT_UNDEFINED;
}

VkFramebuffer GfxDevice::CreateVkFramebuffer(VkRenderPass vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height)
{
	VkFramebuffer framebuffer;

	VkImageView attachments[2];
	attachments[0] = color;
	attachments[1] = depth;

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.flags = 0;
	frameBufferCreateInfo.renderPass = vkRenderPass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = attachments;
	frameBufferCreateInfo.width = width;
	frameBufferCreateInfo.height = height;
	frameBufferCreateInfo.layers = 1;

	VK_CHECK_RESULT(vkCreateFramebuffer(m_VKDevice->device, &frameBufferCreateInfo, nullptr, &framebuffer));

	return framebuffer;
}

VkSemaphore GfxDevice::CreateVKSemaphore()
{
	VkSemaphore semaphore;

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	VK_CHECK_RESULT(vkCreateSemaphore(m_VKDevice->device, &semaphoreCreateInfo, nullptr, &semaphore));

	return semaphore;
}

VkFence GfxDevice::CreateVKFence(bool signaled)
{
	VkFence fence;
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VK_CHECK_RESULT(vkCreateFence(m_VKDevice->device, &fenceCreateInfo, nullptr, &fence));

	return fence;
}
