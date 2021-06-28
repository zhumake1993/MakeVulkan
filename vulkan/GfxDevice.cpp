#include "GfxDevice.h"
#include "Tools.h"
#include "DeviceProperties.h"
#include "GlobalSettings.h"

#include "VKTools.h"
#include "VKFence.h"

#include "VKContex.h"
#include "VKSwapChain.h"

#include "VKMemory.h"
#include "VKBuffer.h"

#include "VKCommandPool.h"

#include "VKFormat.h"

#include "VKCommandBuffer.h"

#include "GarbageCollector.h"
#include "ImageManager.h"
#include "DescriptorSetManager.h"
#include "PipelineManager.h"
#include "RenderPassManager.h"

#include "VKGpuProgram.h"

#include "ProfilerManager.h"
#include "GPUProfilerManager.h"

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
	m_VKContex = new vk::VKContex();

	m_VKSwapChain = new vk::VKSwapChain(m_VKContex->instance, m_VKContex->physicalDevice, m_VKContex->device, m_VKContex->selectedQueueFamilyIndex);

	auto& dp = GetDeviceProperties();
	auto& gs = GetGlobalSettings();

	dp.Print();
	gs.Print();
	m_VKContex->Print();
	m_VKSwapChain->Print();

	m_GarbageCollector = new vk::GarbageCollector();

	// Memory
	VkDeviceSize finalMemoryAlignment = gs.memoryAlignment;
	// 当linear和optimal资源放在同一块memory中时，需要满足bufferImageGranularity的要求
	// 如果bufferImageGranularity太大（例如，在Nvidia上可能会大于4k），那么会造成比较大的内部内存碎片
	// 这种情况下使用多个MemoryAllocator是个好选择。这里选择相对简单的做法
	VkDeviceSize bufferImageGranularity = dp.deviceProperties.limits.bufferImageGranularity;
	finalMemoryAlignment = ALIGN(finalMemoryAlignment, bufferImageGranularity);
	m_MemoryAllocator = new vk::MemoryAllocator(m_VKContex->device, gs.memoryBlockSize, finalMemoryAlignment);

	// buffer
	m_BufferManager = new vk::BufferManager(m_VKContex->device, *m_MemoryAllocator);

	// image
	m_ImageManager = new ImageManager(m_VKContex->device);

	m_VKCommandPool = new VKCommandPool(m_VKContex->device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_VKContex->selectedQueueFamilyIndex);;

	// FrameResource
	m_FrameResources.resize(FrameResourcesCount);
	for (size_t i = 0; i < FrameResourcesCount; ++i)
	{
		m_FrameResources[i].commandBuffer = new VKCommandBuffer(m_VKContex->device, m_VKCommandPool->commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		m_FrameResources[i].imageAvailableSemaphore = CreateVKSemaphore();
		m_FrameResources[i].finishedRenderingSemaphore = CreateVKSemaphore();
		m_FrameResources[i].fence = CreateVKFence(true);
	}

	m_DescriptorSetManager = new DescriptorSetManager(m_VKContex->device);
	m_PipelineManager = new PipelineManager(m_VKContex->device);
	m_RenderPassManager = new RenderPassManager(m_VKContex->device);

	m_DepthFormat = GetSupportedDepthFormat();

	// CommandBuffer
	m_UploadCommandBuffer = new VKCommandBuffer(m_VKContex->device, m_VKCommandPool->commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	m_GPUProfilerManager = new GPUProfilerManager(m_VKContex->device);
}

GfxDevice::~GfxDevice()
{
	m_GPUProfilerManager->WriteToFile();
	RELEASE(m_GPUProfilerManager);

	vkDestroyDescriptorSetLayout(m_VKContex->device, VKGpuProgram::GetDSLGlobal(), nullptr);
	vkDestroyDescriptorSetLayout(m_VKContex->device, VKGpuProgram::GetDSLPerView(), nullptr);

	RELEASE(m_UploadCommandBuffer);

	RELEASE(m_ImageManager);
	RELEASE(m_DescriptorSetManager);
	RELEASE(m_PipelineManager);
	RELEASE(m_RenderPassManager);

	RELEASE(m_GarbageCollector);

	RELEASE(m_BufferManager);
	RELEASE(m_MemoryAllocator);

	for (size_t i = 0; i < FrameResourcesCount; ++i)
	{
		RELEASE(m_FrameResources[i].commandBuffer);

		vkDestroySemaphore(m_VKContex->device, m_FrameResources[i].imageAvailableSemaphore, nullptr);
		m_FrameResources[i].imageAvailableSemaphore = VK_NULL_HANDLE;

		vkDestroySemaphore(m_VKContex->device, m_FrameResources[i].finishedRenderingSemaphore, nullptr);
		m_FrameResources[i].finishedRenderingSemaphore = VK_NULL_HANDLE;

		vkDestroyFence(m_VKContex->device, m_FrameResources[i].fence, nullptr);
		m_FrameResources[i].fence = VK_NULL_HANDLE;
	}

	RELEASE(m_VKCommandPool);
	
	RELEASE(m_VKSwapChain);
	RELEASE(m_VKContex);
}

void GfxDevice::WaitForPresent()
{
	PROFILER(GfxDevice_WaitForPresent);

	auto& currFrameResource = m_FrameResources[m_FrameResourceIndex];

	VK_CHECK_RESULT(vkWaitForFences(m_VKContex->device, 1, &currFrameResource.fence, VK_TRUE, UINT64_MAX));
	VK_CHECK_RESULT(vkResetFences(m_VKContex->device, 1, &currFrameResource.fence));
}

void GfxDevice::AcquireNextImage()
{
	PROFILER(GfxDevice_AcquireNextImage);

	VkResult result = vkAcquireNextImageKHR(m_VKContex->device, m_VKSwapChain->GetSwapChain(), UINT64_MAX, m_FrameResources[m_FrameResourceIndex].imageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);

	switch (result)
	{
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		LOGE("SwapChain is out of date.\n");
		break;
	default:
		LOG("Problem occurred during swap chain image acquisition!\n");
		assert(false);
	}
}

void GfxDevice::QueueSubmit()
{
	PROFILER(GfxDevice_QueueSubmit);

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

	VK_CHECK_RESULT(vkQueueSubmit(m_VKContex->queue, 1, &submitInfo, currFrameResource.fence));
}

void GfxDevice::QueuePresent()
{
	PROFILER(GfxDevice_QueuePresent);

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_FrameResources[m_FrameResourceIndex].finishedRenderingSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_VKSwapChain->GetSwapChain();
	presentInfo.pImageIndices = &m_ImageIndex;
	presentInfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(m_VKContex->queue, &presentInfo);

	switch (result)
	{
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		LOGE("SwapChain is out of date.\n");
		break;
	default:
		LOG("Problem occurred during image presentation!\n");
		assert(false);
	}
}

//ImageFormat GfxDevice::GetSwapChainFormat()
//{
//	VkSurfaceFormatKHR sf = m_VKSwapChain->GetSurfaceFormat();
//	return vk::VkFormatToImageFormat(sf.format);
//}
//
//Extent2D GfxDevice::GetSwapChainExtent()
//{
//	VkExtent2D extent = m_VKSwapChain->GetExtent2D();
//	return Extent2D(extent.width, extent.height);
//}
//
//ImageFormat GfxDevice::GetDepthFormat()
//{
//	return vk::VkFormatToImageFormat(m_DepthFormat);
//}

VkFormat GfxDevice::GetSwapChainFormat()
{
	return m_VKSwapChain->GetSurfaceFormat().format;
}

VkExtent2D GfxDevice::GetSwapChainExtent()
{
	return m_VKSwapChain->GetExtent2D();
}

VkFormat GfxDevice::GetDepthFormat()
{
	return m_DepthFormat;
}

void GfxDevice::Update()
{
	PROFILER(GfxDevice_Update);

	m_DescriptorSetManager->Update();
	m_PipelineManager->Update();
	m_GarbageCollector->GarbageCollect();

	m_GPUProfilerManager->Update();

	vk::GetFrameManager().IncreaseFrameIndex();
	m_FrameResourceIndex = (m_FrameResourceIndex + 1) % FrameResourcesCount;
}

void GfxDevice::DeviceWaitIdle()
{
	vkDeviceWaitIdle(m_VKContex->device);
}

void GfxDevice::BeginCommandBuffer()
{
	m_FrameResources[m_FrameResourceIndex].commandBuffer->Begin();
}

void GfxDevice::EndCommandBuffer()
{
	m_FrameResources[m_FrameResourceIndex].commandBuffer->End();
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

GfxBuffer * GfxDevice::CreateBuffer(GfxBufferUsage bufferUsage, GfxBufferMode bufferMode, uint64_t size)
{
	return new vk::VulkanBuffer(m_BufferManager, bufferUsage, bufferMode, size);
}

void GfxDevice::UpdateBuffer(GfxBuffer * buffer, void * data, uint64_t offset, uint64_t size)
{
	auto vulkanBuffer = static_cast<vk::VulkanBuffer*>(buffer);

	vulkanBuffer->Update(data, offset, size, m_UploadCommandBuffer, vk::GetFrameManager().GetFrameIndex());

	// todo：低效
	if (m_UploadCommandBuffer->m_NeedSubmit)
	{
		m_UploadCommandBuffer->m_NeedSubmit = false;

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
		VK_CHECK_RESULT(vkQueueSubmit(m_VKContex->queue, 1, &submitInfo, VK_NULL_HANDLE));
		DeviceWaitIdle();
	}
}

void GfxDevice::DeleteBuffer(GfxBuffer * buffer)
{
	RELEASE(static_cast<vk::VulkanBuffer*>(buffer));
}

Image * GfxDevice::CreateImage(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount, float maxAnisotropy)
{
	ASSERT(!(imageTypeMask & kImageSwapChainBit));

	ImageVulkan* imageVulkan = new ImageVulkan();

	imageVulkan->m_ImageTypeMask = imageTypeMask;

	imageVulkan->m_ImageType = VK_IMAGE_TYPE_2D;
	imageVulkan->m_Format = format;
	imageVulkan->m_Width = width;
	imageVulkan->m_Height = height;
	imageVulkan->m_MipLevels = mipLevels;
	imageVulkan->m_LayerCount = layerCount;
	imageVulkan->m_FaceCount = faceCount;
	
	// Usage
	if (imageTypeMask & kImageTransferSrcBit) imageVulkan->m_Usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	if (imageTypeMask & kImageTransferDstBit) imageVulkan->m_Usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if (imageTypeMask & kImageSampleBit) imageVulkan->m_Usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if (imageTypeMask & kImageColorAttachmentBit) imageVulkan->m_Usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (imageTypeMask & kImageDepthAttachmentBit) imageVulkan->m_Usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if (imageTypeMask & kImageInputAttachmentBit) imageVulkan->m_Usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

	// ImageViewType
	if (layerCount == 1)
	{
		if (faceCount == 1)
			imageVulkan->m_ImageViewType = VK_IMAGE_VIEW_TYPE_2D;
		else
			imageVulkan->m_ImageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
	}
	else
	{
		if (faceCount == 1)
			imageVulkan->m_ImageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		else
			imageVulkan->m_ImageViewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	}

	// AspectMask
	if (imageTypeMask & kImageColorAspectBit) imageVulkan->m_AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	if (imageTypeMask & kImageDepthAspectBit) imageVulkan->m_AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	// Image
	imageVulkan->m_Image = m_ImageManager->GetImage(imageVulkan->GetKey());

	// Image Sampler
	if (imageTypeMask & kImageSampleBit)
	{
		ImageSamplerKey imageSamplerKey;
		imageSamplerKey.mipLevels = mipLevels;
		imageSamplerKey.maxAnisotropy = maxAnisotropy;
		imageVulkan->m_ImageSampler = m_ImageManager->GetImageSampler(imageSamplerKey);
	}

	return imageVulkan;
}

Image * GfxDevice::GetSwapchainImage()
{
	ImageVulkan* imageVulkan = new ImageVulkan();

	imageVulkan->m_ImageTypeMask = kImageSwapChainBit;
	imageVulkan->m_Image = new VKImage(VK_NULL_HANDLE);
	imageVulkan->m_Image->view = m_VKSwapChain->GetImageView(m_ImageIndex);

	return imageVulkan;
}

void GfxDevice::UpdateImage(Image * image, void * data, uint64_t size, const std::vector<std::vector<std::vector<uint64_t>>>& offsets)
{
	// 简单起见，假设当前该image的资源并没有被GPU使用中

	ImageVulkan* imageVulkan = static_cast<ImageVulkan*>(image);

	ASSERT(imageVulkan->m_ImageTypeMask & kImageTransferDstBit);

	vk::BufferResource* stagingBuffer = m_BufferManager->CreateTransientStagingBufferResource(size);
	stagingBuffer->Update(data, 0, size);

	m_UploadCommandBuffer->Begin();

	m_UploadCommandBuffer->ImageMemoryBarrier(imageVulkan->m_Image->image, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageVulkan->m_MipLevels, imageVulkan->m_LayerCount, imageVulkan->m_FaceCount);

	m_UploadCommandBuffer->CopyBufferToImage(stagingBuffer->GetBuffer(), imageVulkan->m_Image->image, imageVulkan->m_Width, imageVulkan->m_Height, offsets);

	m_UploadCommandBuffer->ImageMemoryBarrier(imageVulkan->m_Image->image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, imageVulkan->m_MipLevels, imageVulkan->m_LayerCount, imageVulkan->m_FaceCount);

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
	VK_CHECK_RESULT(vkQueueSubmit(m_VKContex->queue, 1, &submitInfo, VK_NULL_HANDLE));

	DeviceWaitIdle();
}

void GfxDevice::ReleaseImage(Image * image)
{
	ImageVulkan* imageVulkan = static_cast<ImageVulkan*>(image);

	if(!(imageVulkan->m_ImageTypeMask & kImageSwapChainBit))
		m_ImageManager->ReleaseImage(imageVulkan->GetKey(), imageVulkan->m_Image);
}

RenderPass * GfxDevice::CreateRenderPass(RenderPassKey& renderPassKey)
{
	return new RenderPassVulkan(renderPassKey);
}

void GfxDevice::BeginRenderPass(RenderPass* renderPass, Rect2D& renderArea, std::vector<VkClearValue>& clearValues)
{
	PROFILER(GfxDevice_BeginRenderPass);

	m_CurrentRenderPass = static_cast<RenderPassVulkan*>(renderPass);

	m_CurrentRenderPass->m_RenderPass = m_RenderPassManager->GetRenderPass(m_CurrentRenderPass->GetKey());
	m_CurrentRenderPass->m_SubpassIndex = 0;

	// Framebuffer

	VKFramebuffer* framebuffer = new VKFramebuffer(m_VKContex->device);

	std::vector<Image*>& images = m_CurrentRenderPass->GetImages();
	std::vector<VkImageView> views(images.size());
	for (size_t i = 0; i < images.size(); i++)
	{
		ImageVulkan* imageVulkan = static_cast<ImageVulkan*>(images[i]);
		views[i] = imageVulkan->m_Image->view;

		imageVulkan->m_Image->Use();
	}

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.flags = 0;
	frameBufferCreateInfo.renderPass = m_CurrentRenderPass->m_RenderPass->renderPass;
	frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(views.size());
	frameBufferCreateInfo.pAttachments = views.data();
	frameBufferCreateInfo.width = m_CurrentRenderPass->GetWidth();
	frameBufferCreateInfo.height = m_CurrentRenderPass->GetHeight();
	frameBufferCreateInfo.layers = 1;

	VK_CHECK_RESULT(vkCreateFramebuffer(m_VKContex->device, &frameBufferCreateInfo, nullptr, &framebuffer->framebuffer));

	VkRect2D area = {};
	area.offset.x = renderArea.x;
	area.offset.y = renderArea.y;
	area.extent.width = renderArea.width;
	area.extent.height = renderArea.height;

	m_FrameResources[m_FrameResourceIndex].commandBuffer->BeginRenderPass(m_CurrentRenderPass->m_RenderPass->renderPass, framebuffer->framebuffer, area, clearValues);

	m_CurrentRenderPass->m_RenderPass->Use();
	m_RenderPassManager->ReleaseRenderPass(m_CurrentRenderPass->GetKey(), m_CurrentRenderPass->m_RenderPass);

	framebuffer->Use();
	m_GarbageCollector->Add(framebuffer);
}

void GfxDevice::NextSubpass()
{
	m_FrameResources[m_FrameResourceIndex].commandBuffer->NextSubpass();

	m_CurrentRenderPass->m_SubpassIndex++;
}

void GfxDevice::EndRenderPass()
{
	m_FrameResources[m_FrameResourceIndex].commandBuffer->EndRenderPass();
}

GpuProgram * GfxDevice::CreateGpuProgram(GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode)
{
	return new VKGpuProgram(m_VKContex->device, parameters, vertCode, fragCode);
}

void GfxDevice::SetPass(GpuProgram * gpuProgram, RenderState * renderState, void* scdata)
{
	VKGpuProgram* vkGpuProgram = static_cast<VKGpuProgram*>(gpuProgram);

	m_PipelineManager->SetPipelineKey(vkGpuProgram, renderState, scdata, m_CurrentRenderPass->m_RenderPass->renderPass, m_CurrentRenderPass->m_SubpassIndex);
}

void GfxDevice::BindUniformBuffer(GpuProgram * gpuProgram, int set, int binding, void * data, uint64_t size)
{
	VKGpuProgram* vkGpuProgram = static_cast<VKGpuProgram*>(gpuProgram);

	VkDescriptorSet descriptorSet;
	switch (set)
	{
	case 0:descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(vkGpuProgram->GetDSLGlobal()); break;
	case 1:descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(vkGpuProgram->GetDSLPerView()); break;
	case 3:descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(vkGpuProgram->GetDSLPerDraw()); break;
	default:LOGE("wrong set.");
	}

	if (size > 0)
	{
		vk::BufferResource* stagingBuffer = m_BufferManager->CreateTransientUniformBufferResource(size);
		stagingBuffer->Update(data, 0, size);

		UpdateDescriptorSetBuffer(descriptorSet, binding, stagingBuffer->GetBuffer());

		m_FrameResources[m_FrameResourceIndex].commandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkGpuProgram->GetPipelineLayout(), set, descriptorSet, { 0 });
	}
	else
	{
		m_FrameResources[m_FrameResourceIndex].commandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkGpuProgram->GetPipelineLayout(), set, descriptorSet);
	}
}

void GfxDevice::BindImage(GpuProgram * gpuProgram, int binding, void * image)
{
	VKGpuProgram* vkGpuProgram = static_cast<VKGpuProgram*>(gpuProgram);

	VkDescriptorSet descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(vkGpuProgram->GetDSLPerMaterial());

	UpdateDescriptorSetImage(descriptorSet, binding, static_cast<ImageVulkan*>(image));

	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkGpuProgram->GetPipelineLayout(), 2, descriptorSet);
}

void GfxDevice::BindMaterial(GpuProgram * gpuProgram, MaterialBindData & data)
{
	VKGpuProgram* vkGpuProgram = static_cast<VKGpuProgram*>(gpuProgram);

	VkDescriptorSet descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(vkGpuProgram->GetDSLPerMaterial());

	std::vector<uint32_t> offsets;
	
	for (auto& uniform : data.uniformBufferBindings)
	{
		vk::VulkanBuffer* vulkanBuffer = static_cast<vk::VulkanBuffer*>(uniform.buffer);
		VkBuffer buffer = vulkanBuffer->AccessBuffer();

		UpdateDescriptorSetBuffer(descriptorSet, uniform.binding, buffer);

		offsets.push_back(0);
	}

	for (auto& image : data.imageBindings)
	{
		UpdateDescriptorSetImage(descriptorSet, image.binding, static_cast<ImageVulkan*>(image.image));
	}

	uint32_t inputAttachmentIndex = 0;
	for (auto& inputAttachment : data.inputAttachmentBindings)
	{
		UpdateDescriptorSetInputAttachment(descriptorSet, inputAttachment.binding, m_CurrentRenderPass->GetInputAttachmentImageView(inputAttachmentIndex));
		inputAttachmentIndex++;
	}

	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkGpuProgram->GetPipelineLayout(), 2, descriptorSet, offsets);
}

void GfxDevice::BindMeshBuffer(GfxBuffer * vertexBuffer, GfxBuffer * indexBuffer, VertexDescription * vertexDescription, VkIndexType indexType)
{
	VkPipeline pipeline = m_PipelineManager->CreatePipeline(vertexDescription);
	m_FrameResources[m_FrameResourceIndex].commandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	if (vertexDescription)
	{
		VkBuffer vBuffer = static_cast<vk::VulkanBuffer*>(vertexBuffer)->AccessBuffer();
		VkBuffer iBuffer = static_cast<vk::VulkanBuffer*>(indexBuffer)->AccessBuffer();

		m_FrameResources[m_FrameResourceIndex].commandBuffer->BindVertexBuffer(0, vBuffer);
		m_FrameResources[m_FrameResourceIndex].commandBuffer->BindIndexBuffer(iBuffer, indexType);
	}
}

void GfxDevice::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	m_FrameResources[m_FrameResourceIndex].commandBuffer->DrawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void GfxDevice::DrawBatch(DrawBatchs & drawBatchs)
{
	VKGpuProgram* vkGpuProgram = static_cast<VKGpuProgram*>(drawBatchs.gpuProgram);

	// DescriptorSet

	VkDescriptorSet descriptorSet = m_DescriptorSetManager->AllocateDescriptorSet(vkGpuProgram->GetDSLPerDraw());

	vk::BufferResource* stagingBuffer = m_BufferManager->CreateTransientUniformBufferResource(drawBatchs.uniformSize);
	stagingBuffer->Update(drawBatchs.uniformData, 0, drawBatchs.uniformSize);

	UpdateDescriptorSetBuffer(descriptorSet, drawBatchs.uniformBinding, stagingBuffer->GetBuffer(), 0, drawBatchs.alignedUniformSize);

	// Draw
	uint32_t uniformOffset = 0;
	for (size_t i = 0; i < drawBatchs.drawItems.size(); i++)
	{
		DrawItem& drawItem = drawBatchs.drawItems[i];
		DrawBuffer& drawBuffer = drawBatchs.drawBuffers[drawItem.drawBufferIndex];

		VkPipeline pipeline = m_PipelineManager->CreatePipeline(static_cast<VertexDescription*>(drawBuffer.vertexDescription));
		m_FrameResources[m_FrameResourceIndex].commandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		// Bind DescriptorSet
		m_FrameResources[m_FrameResourceIndex].commandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkGpuProgram->GetPipelineLayout(), 3, descriptorSet, { uniformOffset });
		uniformOffset += static_cast<uint32_t>(drawBatchs.alignedUniformSize);

		VkBuffer vBuffer = static_cast<vk::VulkanBuffer*>(drawBuffer.vertexBuffer)->AccessBuffer();
		VkBuffer iBuffer = static_cast<vk::VulkanBuffer*>(drawBuffer.indexBuffer)->AccessBuffer();

		m_FrameResources[m_FrameResourceIndex].commandBuffer->BindVertexBuffer(0, vBuffer);
		m_FrameResources[m_FrameResourceIndex].commandBuffer->BindIndexBuffer(iBuffer, drawBuffer.indexType);

		m_FrameResources[m_FrameResourceIndex].commandBuffer->DrawIndexed(drawBuffer.indexCount, 1, 0, 0, 0);
	}
}

void GfxDevice::PushConstants(GpuProgram * gpuProgram, void * data, uint32_t offset, uint32_t size)
{
	VKGpuProgram* vkGpuProgram = static_cast<VKGpuProgram*>(gpuProgram);

	m_FrameResources[m_FrameResourceIndex].commandBuffer->PushConstants(vkGpuProgram->GetPipelineLayout(), vkGpuProgram->GetGpuParameters().pushConstantStage, offset, size, data);
}

void GfxDevice::ResetTimeStamp()
{
	m_GPUProfilerManager->Reset(m_FrameResources[m_FrameResourceIndex].commandBuffer);
}

void GfxDevice::WriteTimeStamp(std::string name)
{
	m_GPUProfilerManager->WriteTimeStamp(m_FrameResources[m_FrameResourceIndex].commandBuffer, name);
}

void GfxDevice::ResolveTimeStamp()
{
	m_GPUProfilerManager->ResolveTimeStamp();
}

std::string GfxDevice::GetLastGPUTimeStamp()
{
	return m_GPUProfilerManager->GetLastFrameView().ToString();
}

vk::GarbageCollector * GfxDevice::GetGarbageCollector()
{
	return m_GarbageCollector;
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
		vkGetPhysicalDeviceFormatProperties(m_VKContex->physicalDevice, format, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			return format;
		}
	}

	LOGE("Can not find supported depth format");
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

	VK_CHECK_RESULT(vkCreateFramebuffer(m_VKContex->device, &frameBufferCreateInfo, nullptr, &framebuffer));

	return framebuffer;
}

VkSemaphore GfxDevice::CreateVKSemaphore()
{
	VkSemaphore semaphore;

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	VK_CHECK_RESULT(vkCreateSemaphore(m_VKContex->device, &semaphoreCreateInfo, nullptr, &semaphore));

	return semaphore;
}

VkFence GfxDevice::CreateVKFence(bool signaled)
{
	VkFence fence;
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VK_CHECK_RESULT(vkCreateFence(m_VKContex->device, &fenceCreateInfo, nullptr, &fence));

	return fence;
}

void GfxDevice::UpdateDescriptorSetBuffer(VkDescriptorSet descriptorSet, uint32_t binding, VkBuffer buffer, uint64_t offset, uint64_t range)
{
	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = buffer;
	bufferInfo.offset = offset;
	bufferInfo.range = range;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	writeDescriptorSet.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(m_VKContex->device, 1, &writeDescriptorSet, 0, nullptr);
}

void GfxDevice::UpdateDescriptorSetImage(VkDescriptorSet descriptorSet, uint32_t binding, ImageVulkan * imageVulkan)
{
	VkDescriptorImageInfo imageInfo;
	imageInfo.sampler = imageVulkan->m_ImageSampler->sampler;
	imageInfo.imageView = imageVulkan->m_Image->view;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(m_VKContex->device, 1, &writeDescriptorSet, 0, nullptr);

	imageVulkan->m_Image->Use();
}

void GfxDevice::UpdateDescriptorSetInputAttachment(VkDescriptorSet descriptorSet, uint32_t binding, VkImageView view)
{
	VkDescriptorImageInfo imageInfo;
	imageInfo.sampler = VK_NULL_HANDLE;
	imageInfo.imageView = view;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = nullptr;
	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	writeDescriptorSet.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(m_VKContex->device, 1, &writeDescriptorSet, 0, nullptr);
}
