#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "DescriptorSetTypes.h"
#include "Gui.h"

class Engine : public NonCopyable
{

public:

	Engine();
	virtual ~Engine();

	void CleanUpEngine();
	void InitEngine();
	void TickEngine();

protected:

	virtual void CleanUp() = 0;
	virtual void Init() = 0;
	virtual void Tick() = 0;
	virtual void TickUI() = 0;
	virtual void RecordCommandBuffer(VKCommandBuffer* vkCommandBuffer) = 0;

	VKFramebuffer* RebuildFramebuffer(VKRenderPass* vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);
	void UpdatePassUniformBuffer(void* data);
	void UpdateObjectUniformBuffer(void* data, uint32_t index);
	VKBuffer* GetCurrPassUniformBuffer();
	VKBuffer* GetCurrObjectUniformBuffer();
	uint32_t GetUBODynamicAlignment();

private:

	void WaitForPresent();
	void Present();

protected:

	Imgui* m_Imgui;

private:

	struct FrameResource {
		VKFramebuffer* framebuffer;
		VKCommandBuffer* commandBuffer;
		VKSemaphore* imageAvailableSemaphore;
		VKSemaphore* finishedRenderingSemaphore;
		VKFence* fence;

		FrameResource() :
			framebuffer(nullptr),
			commandBuffer(nullptr),
			imageAvailableSemaphore(nullptr),
			finishedRenderingSemaphore(nullptr),
			fence(nullptr) {
		}
	};

	VKCommandPool* m_VKCommandPool;

	size_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;
	std::vector<VKBuffer*> m_PassUniformBuffers;
	const uint32_t m_ObjectUniformNum = 100;
	std::vector<VKBuffer*> m_ObjectUniformBuffers;
	uint32_t m_UBODynamicAlignment;
};