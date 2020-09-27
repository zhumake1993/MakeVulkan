#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "DescriptorSetTypes.h"
#include "Gui.h"
#include "TimeMgr.h"

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
	virtual void Tick(float deltaTime) = 0;
	virtual void TickUI() = 0;
	virtual void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer) = 0;

	VulkanFramebuffer* RebuildFramebuffer(VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);
	void UpdatePassUniformBuffer(void* data);
	void UpdateObjectUniformBuffer(void* data, uint32_t index);
	VulkanBuffer* GetCurrPassUniformBuffer();
	VulkanBuffer* GetCurrObjectUniformBuffer();

private:

	void WaitForPresent();
	void Present();

protected:

	Imgui* m_Imgui;

private:

	struct FrameResource {
		VulkanFramebuffer* framebuffer;
		VulkanCommandBuffer* commandBuffer;
		VulkanSemaphore* imageAvailableSemaphore;
		VulkanSemaphore* finishedRenderingSemaphore;
		VulkanFence* fence;

		FrameResource() :
			framebuffer(nullptr),
			commandBuffer(nullptr),
			imageAvailableSemaphore(nullptr),
			finishedRenderingSemaphore(nullptr),
			fence(nullptr) {
		}
	};

	VulkanCommandPool* m_VulkanCommandPool;

	size_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;
	std::vector<VulkanBuffer*> m_PassUniformBuffers;
	const uint32_t m_ObjectUniformNum = 100;
	std::vector<VulkanBuffer*> m_ObjectUniformBuffers;

	TimeMgr m_TimeMgr;
};