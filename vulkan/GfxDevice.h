#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "GfxTypes.h"
#include "GpuProgram.h"

struct VKInstance;
struct VKSurface;
struct VKDevice;
struct VKSwapChain;
struct VKCommandPool;

struct VKCommandBuffer;
struct VKRenderPass;
struct VKBuffer;
struct VKImage;
struct VKImageView;

class GarbageCollector;
class BufferManager;
class ImageManager;
class DescriptorSetManager;
class PipelineManager;
struct PipelineCI;

class GPUProfilerManager;

class Buffer;
class Image;
class GpuProgram;
class Shader;
class Material;

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

	void BeginRenderPass(Rect2D& renderArea, Color& clearColor, DepthStencil& clearDepthStencil);
	void EndRenderPass();

	void SetViewport(Viewport& viewport);
	void SetScissor(Rect2D& scissorArea);

	Buffer* CreateBuffer(BufferUsageType bufferUsage, MemoryPropertyType memoryProp, uint64_t size);
	void UpdateBuffer(Buffer* buffer, void* data, uint64_t offset, uint64_t size);
	void FlushBuffer(Buffer* buffer);
	void ReleaseBuffer(Buffer* buffer);

	Image* CreateImage(ImageType imageType, VkFormat format, uint32_t width, uint32_t height);
	void UpdateImage(Image* image, void* data, uint64_t size);
	void ReleaseImage(Image* image);

	GpuProgram* CreateGpuProgram(GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode);

	void SetPass(GpuProgram* gpuProgram, RenderState* renderState, void* scdata);

	void BindShaderResources(GpuProgram* gpuProgram, int set, ShaderBindings shaderBindings);

	void BindMeshBuffer(Buffer* vertexBuffer, Buffer* indexBuffer, VertexDescription* vertexDescription, VkIndexType indexType = VK_INDEX_TYPE_UINT32);

	void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0);

	void PushConstants(GpuProgram* gpuProgram, void* data, uint32_t offset, uint32_t size);

	void ResetTimeStamp();
	void WriteTimeStamp(std::string name);
	void ResolveTimeStamp();
	std::string GetLastGPUTimeStamp();

private:

	VkFormat GetSupportedDepthFormat();

	VkFramebuffer CreateVkFramebuffer(VkRenderPass vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);

	VkSemaphore CreateVKSemaphore();

	VkFence CreateVKFence(bool signaled);

private:

	VKInstance* m_VKInstance = nullptr;
	VKSurface* m_VKSurface = nullptr;
	VKDevice* m_VKDevice = nullptr;
	VKSwapChain* m_VKSwapChain = nullptr;
	VKCommandPool* m_VKCommandPool = nullptr;

	uint32_t m_FrameIndex = 0;
	uint32_t m_FrameResourceIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	// ��Դ������
	GarbageCollector* m_GarbageCollector = nullptr; // GC
	BufferManager* m_BufferManager = nullptr;
	ImageManager* m_ImageManager = nullptr;
	DescriptorSetManager* m_DescriptorSetManager = nullptr;
	PipelineManager* m_PipelineManager = nullptr;

	// Depth
	VKImage* m_DepthImage;
	VKImageView* m_DepthView;

	// RenderPass
	VKRenderPass* m_VKRenderPass = nullptr;

	// SwapChain�е�image�������ܲ�������FrameResourcesCount������Ҫ��������Framebuffer
	uint32_t m_ImageIndex;
	std::vector<VkFramebuffer> m_Framebuffers;

	// ���ڴ�����
	VKCommandBuffer* m_UploadCommandBuffer;

	GPUProfilerManager* m_GPUProfilerManager;
};

void CreateGfxDevice();
GfxDevice& GetGfxDevice();
void ReleaseGfxDevice();