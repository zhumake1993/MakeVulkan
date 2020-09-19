#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "DescriptorSetTypes.h"

class DescriptorSetMgr;

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
	virtual void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer) = 0;

	VulkanFramebuffer* RebuildFramebuffer(VulkanRenderPass* vulkanRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);
	void UpdatePassUniformBuffer(void* data);
	void UpdateObjectUniformBuffer(void* data, uint32_t index);
	VulkanBuffer* GetCurrPassUniformBuffer();
	VulkanBuffer* GetCurrObjectUniformBuffer();

	// DescriptorSet
	VkDescriptorSetLayout CreateDescriptorSetLayout(DSLBindings& bindings);
	VkDescriptorSet GetDescriptorSet(VkDescriptorSetLayout layout);
	void UpdateDescriptorSet(VkDescriptorSet set, DesUpdateInfos& infos);

private:

	void WaitForPresent();
	void Present();

protected:

	//

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

	DescriptorSetMgr* m_DescriptorSetMgr;
};