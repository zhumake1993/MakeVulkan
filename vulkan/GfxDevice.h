#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "GfxTypes.h"

struct VKInstance;
struct VKSurface;
struct VKDevice;
struct VKSwapChain;
struct VKCommandPool;

struct VKCommandBuffer;
struct VKRenderPass;
class VKImage;
class BufferManager;
class DescriptorSetManager;
class PipelineManager;

struct PipelineCI;

class Buffer;
class Image;
class Shader;
class GpuProgram;

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

	Image* CreateImage(ImageType imageType, VkFormat format, uint32_t width, uint32_t height);
	void UpdateImage(Image* image, void* data, uint64_t size);

	GpuProgram* CreateGpuProgram(GpuParameters& parameters, const std::vector<char>& vertCode, const std::vector<char>& fragCode);

	void BindUniformGlobal(void* data, uint64_t size);
	void BindUniformPerView(void* data, uint64_t size);

	void SetShader(Shader* shader);

	void BindUniformPerMaterial(Shader* shader, void* data, uint64_t size);
	void BindUniformPerDraw(Shader* shader, void* data, uint64_t size);

	void DrawBuffer(Buffer* vertexBuffer, Buffer* indexBuffer, uint32_t indexCount, VertexDescription& vertexDescription);

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

	VKRenderPass* m_VKRenderPass = nullptr;

	// depth
	VkFormat m_DepthFormat;
	VKImage* m_DepthImage;

	uint32_t m_FrameIndex = 0;
	uint32_t m_FrameResourceIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	// SwapChain中的image数量可能并不等于FrameResourcesCount，所以要单独处理Framebuffer
	uint32_t m_ImageIndex;
	std::vector<VkFramebuffer> m_Framebuffers;

	// 用于传数据
	VKCommandBuffer* m_UploadCommandBuffer;

	// 资源管理器

	// 管理Buffer
	BufferManager* m_BufferManager = nullptr;

	// 管理DescriptorPool, DescriptorSetLayout, DescriptorSet
	DescriptorSetManager* m_DescriptorSetManager = nullptr;

	// 管理PipelineLayout，Pipeline
	PipelineManager* m_PipelineManager = nullptr;
};

void CreateGfxDevice();
GfxDevice& GetGfxDevice();
void ReleaseGfxDevice();