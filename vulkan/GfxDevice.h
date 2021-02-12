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

	Buffer* CreateBuffer(BufferType bufferType, uint64_t size);
	void UpdateBuffer(Buffer* buffer, void* data, uint64_t size);
	void ReleaseBuffer(Buffer* buffer);

	Image* CreateImage(ImageType imageType, VkFormat format, uint32_t width, uint32_t height);
	void UpdateImage(Image* image, void* data, uint64_t size);
	void ReleaseImage(Image* image);

	GpuProgram* CreateGpuProgram(GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode);

	void SetPass(GpuProgram* gpuProgram, RenderStatus& renderStatus);

	void BindUniformBuffer(GpuProgram* gpuProgram, int set, int binding, Buffer* buffer);
	void BindUniformBuffer(GpuProgram* gpuProgram, int set, int binding, void* data, uint64_t size);

	void BindImage(GpuProgram* gpuProgram, int set, int binding, Image* image);

	void DrawBuffer(Buffer* vertexBuffer, Buffer* indexBuffer, uint32_t indexCount, VertexDescription& vertexDescription);

private:

	VkFormat GetSupportedDepthFormat();

	VkFramebuffer CreateVkFramebuffer(VkRenderPass vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);

	VkSemaphore CreateVKSemaphore();

	VkFence CreateVKFence(bool signaled);

	void BindUniformBuffer(GpuProgram* gpuProgram, int set, int binding, VKBuffer* buffer);

private:

	VKInstance* m_VKInstance = nullptr;
	VKSurface* m_VKSurface = nullptr;
	VKDevice* m_VKDevice = nullptr;
	VKSwapChain* m_VKSwapChain = nullptr;
	VKCommandPool* m_VKCommandPool = nullptr;

	uint32_t m_FrameIndex = 0;
	uint32_t m_FrameResourceIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	// 资源管理器
	GarbageCollector* m_GarbageCollector = nullptr; // GC
	BufferManager* m_BufferManager = nullptr; // 管理Buffer
	ImageManager* m_ImageManager = nullptr; // 管理Image
	DescriptorSetManager* m_DescriptorSetManager = nullptr; // 管理DescriptorPool, DescriptorSetLayout, DescriptorSet
	PipelineManager* m_PipelineManager = nullptr; // 管理PipelineLayout，Pipeline

	// Depth
	VKImage* m_DepthImage;
	VKImageView* m_DepthView;

	// RenderPass
	VKRenderPass* m_VKRenderPass = nullptr;

	// SwapChain中的image数量可能并不等于FrameResourcesCount，所以要单独处理Framebuffer
	uint32_t m_ImageIndex;
	std::vector<VkFramebuffer> m_Framebuffers;

	// 用于传数据
	VKCommandBuffer* m_UploadCommandBuffer;
};

void CreateGfxDevice();
GfxDevice& GetGfxDevice();
void ReleaseGfxDevice();