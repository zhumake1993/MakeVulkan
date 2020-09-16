#include "Engine.h"

#include "VulkanDriver.h"

#include "VulkanCommandPool.h"

#include "VulkanCommandBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "VulkanFramebuffer.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"

#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

#include "Tools.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::CleanUpEngine()
{
	auto& driver = GetVulkanDriver();
	driver.WaitIdle();

	// 先清理子类
	CleanUp();

	ReleaseVulkanDriver();
}

void Engine::InitEngine()
{
	CreateVulkanDriver();

	auto& driver = GetVulkanDriver();

	// 后初始化子类
	Init();
}

void Engine::TickEngine()
{
	Tick();

	auto& driver = GetVulkanDriver();

	driver.WaitForPresent();

	RecordCommandBuffer(driver.GetCurrVulkanCommandBuffer());

	driver.Present();
}