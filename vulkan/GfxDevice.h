#pragma once

#include <vector>
#include "VKIncludes.h"
#include "NonCopyable.h"

#include "GfxTypes.h"
#include "GpuProgram.h"
#include "GfxDeviceObjects.h"

struct VKCommandPool;

struct VKCommandBuffer;
class VKRenderPass;
struct VKBuffer;
class VKImage;
class ImageVulkan;

class GarbageCollector;
class ImageManager;
class DescriptorSetManager;
class PipelineManager;
struct PipelineCI;

class Attachment;
class RenderPass;
class RenderPassVulkan;
class RenderPassManager;

class GPUProfilerManager;

class Image;
class GpuProgram;
class Shader;
class Material;



class GfxBuffer;

namespace vk
{
	class VKContex;
	class VKSwapChain;
	class MemoryAllocator;
	class BufferManager;
}

class GfxDevice : public NonCopyable
{

	struct FrameResource
	{
		VKCommandBuffer* commandBuffer;
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore finishedRenderingSemaphore;
		VkFence fence;

		FrameResource() :
			commandBuffer(nullptr),
			imageAvailableSemaphore(VK_NULL_HANDLE),
			finishedRenderingSemaphore(VK_NULL_HANDLE),
			fence(VK_NULL_HANDLE)
		{
		}
	};

public:

	GfxDevice();
	virtual ~GfxDevice();

	void WaitForPresent();
	void AcquireNextImage();
	void QueueSubmit();
	void QueuePresent();

	void Update();

	void DeviceWaitIdle();

	void BeginCommandBuffer();
	void EndCommandBuffer();

	void SetViewport(Viewport& viewport);
	void SetScissor(Rect2D& scissorArea);
	
	// Buffer
	GfxBuffer* CreateBuffer(GfxBufferUsage bufferUsage, GfxBufferMode bufferMode, uint64_t size);
	void UpdateBuffer(GfxBuffer* buffer, void* data, uint64_t offset, uint64_t size);

	void FlushBuffer(GfxBuffer* buffer);
	void ReleaseBuffer(GfxBuffer* buffer);

	Image* CreateImage(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount, float maxAnisotropy = 1);
	Image* GetSwapchainImage();
	void UpdateImage(Image* image, void* data, uint64_t size, const std::vector<std::vector<std::vector<uint64_t>>>& offsets);
	void ReleaseImage(Image* image);

	RenderPass* CreateRenderPass(RenderPassKey& renderPassKey);
	void BeginRenderPass(RenderPass* renderPass, Rect2D& renderArea, std::vector<VkClearValue>& clearValues);
	void NextSubpass();
	void EndRenderPass();

	GpuProgram* CreateGpuProgram(GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode);

	void SetPass(GpuProgram* gpuProgram, RenderState* renderState, void* scdata);

	void BindUniformBuffer(GpuProgram* gpuProgram, int set, int binding, void* data, uint64_t size);

	void BindImage(GpuProgram* gpuProgram, int binding, void* image);

	void BindMaterial(GpuProgram* gpuProgram, MaterialBindData& data);

	void BindMeshBuffer(Buffer* vertexBuffer, Buffer* indexBuffer, VertexDescription* vertexDescription, VkIndexType indexType = VK_INDEX_TYPE_UINT32);

	void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0);

	void DrawBatch(DrawBatchs& drawBatchs);

	void PushConstants(GpuProgram* gpuProgram, void* data, uint32_t offset, uint32_t size);

	void ResetTimeStamp();
	void WriteTimeStamp(std::string name);
	void ResolveTimeStamp();
	std::string GetLastGPUTimeStamp();

	GarbageCollector* GetGarbageCollector();

private:

	VkFormat GetSupportedDepthFormat();

	VkFramebuffer CreateVkFramebuffer(VkRenderPass vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);

	VkSemaphore CreateVKSemaphore();

	VkFence CreateVKFence(bool signaled);

	void UpdateDescriptorSetBuffer(VkDescriptorSet descriptorSet, uint32_t binding, VKBuffer* vkBuffer, uint64_t offset = 0, uint64_t range = VK_WHOLE_SIZE);
	void UpdateDescriptorSetImage(VkDescriptorSet descriptorSet, uint32_t binding, ImageVulkan* imageVulkan);
	void UpdateDescriptorSetInputAttachment(VkDescriptorSet descriptorSet, uint32_t binding, VkImageView view); // InputAttachment只需要view

private:

	vk::VKContex* m_VKContex = nullptr;
	vk::VKSwapChain* m_VKSwapChain = nullptr;

	GarbageCollector* m_GarbageCollector = nullptr;
	vk::MemoryAllocator* m_MemoryAllocator = nullptr;
	vk::BufferManager* m_BufferManager = nullptr;
	ImageManager* m_ImageManager = nullptr;

	// todo
	VKCommandPool* m_VKCommandPool = nullptr;

	// todo:变成动态
	uint32_t m_FrameResourceIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	// todo
	DescriptorSetManager* m_DescriptorSetManager = nullptr;
	PipelineManager* m_PipelineManager = nullptr;
	RenderPassManager* m_RenderPassManager = nullptr;

	// todo
	// SwapChain中的image数量可能并不等于FrameResourcesCount，所以要单独处理Framebuffer
	uint32_t m_ImageIndex;

	// 当前的RenderPass
	RenderPassVulkan* m_CurrentRenderPass = nullptr;

	// 用于传数据
	VKCommandBuffer* m_UploadCommandBuffer;

	GPUProfilerManager* m_GPUProfilerManager;

	VkFormat m_DepthFormat;
};

void CreateGfxDevice();
GfxDevice& GetGfxDevice();
void ReleaseGfxDevice();