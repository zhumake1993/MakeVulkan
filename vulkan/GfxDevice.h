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
class VKBuffer;

class Buffer;
class Image;

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
	~GfxDevice();

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

	uint32_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	// SwapChain中的image数量可能并不等于FrameResourcesCount，所以要单独处理Framebuffer
	uint32_t m_ImageIndex;
	std::vector<VkFramebuffer> m_Framebuffers;

	// staging
	VKCommandBuffer* m_UploadCommandBuffer;
	const uint32_t m_StagingBufferSize = 10 * 1024 * 1024;
	VKBuffer* m_StagingBuffer;
};

void CreateGfxDevice();
GfxDevice& GetGfxDevice();
void ReleaseGfxDevice();