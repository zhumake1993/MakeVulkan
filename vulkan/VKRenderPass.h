#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "GfxDeviceObjects.h"
#include "VKResource.h"

class ImageManager;
class VKImage;
class VKImageView;
class GarbageCollector;

class VKRenderPass : public VKResource
{
public:

	VKRenderPass(VkDevice vkDevice, RenderPassDesc& renderPassDesc, VkImageView swapChainImageView, ImageManager* imageManager, GarbageCollector* gc);
	virtual ~VKRenderPass();

	VkRenderPass GetRenderPass();
	VkFramebuffer GetFramebuffer();

	uint32_t GetSubPassIndex();
	void NextSubpass();
	VKImageView* GetInputAttachmentImageView(uint32_t index);

private:

	struct AttachmentReference
	{
		std::vector<VkAttachmentReference> inputs;
		std::vector<VkAttachmentReference> colors;
		VkAttachmentReference depthStencil;
	};

	void ConfigAttachmentDescriptions(std::vector<VkAttachmentDescription>& attachmentDescriptions, RenderPassDesc& renderPassDesc);
	void ConfigSubpassDescription(std::vector<AttachmentReference>& attachmentReferences, std::vector<VkSubpassDescription>& subpassDescriptions, RenderPassDesc& renderPassDesc);
	void ConfigSubpassDependencies(std::vector<VkSubpassDependency>& dependencies, RenderPassDesc& renderPassDesc);
	void ConfigImage(RenderPassDesc& renderPassDesc, VkImageView swapChainImageView, ImageManager* imageManager);

private:

	std::vector<VKImage*> m_Images;
	std::vector<VKImageView*> m_ImageViews;

	VkImageView m_SwapChainImageView;

	VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

	uint32_t m_SubPassIndex = 0;

	ImageManager* m_ImageManager = nullptr;

	GarbageCollector* m_GarbageCollector = nullptr;

	RenderPassDesc m_RenderPassDesc;

	VkDevice m_Device = VK_NULL_HANDLE;
};

class RenderPassCache
{
public:

	RenderPassCache();
	~RenderPassCache();

	void Release();

	VKRenderPass* GetRenderPass(RenderPassDesc& renderPassDesc);

private:

	std::unordered_map<RenderPassDesc, VKRenderPass*, RenderPassHash> m_RenderPassCache;
};