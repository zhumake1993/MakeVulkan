#include "Engine.h"
#include "VulkanDriver.h"
#include "Tools.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::CleanUpEngine()
{
	auto driver = GetVulkanDriver();

	driver.WaitIdle();

	// 先清理子类
	CleanUp();

	driver.CleanUp();
}

void Engine::InitEngine()
{
	auto driver = new VulkanDriver();
	SetVulkanDriver(driver);

	driver->Init();

	// 后初始化子类
	Init();
}

void Engine::TickEngine()
{
	Tick();

	auto driver = GetVulkanDriver();

	driver.WaitForPresent();

	auto cmd = driver.GetCurrCommandBuffer();

	RecordCommandBuffer(cmd);

	driver.Present();
}
