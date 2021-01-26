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
struct VKImage;

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

	// SwapChain�е�image�������ܲ�������FrameResourcesCount������Ҫ��������Framebuffer
	uint32_t m_ImageIndex;
	std::vector<VkFramebuffer> m_Framebuffers;
};

void CreateGfxDevice();
GfxDevice& GetGfxDevice();
void ReleaseGfxDevice();