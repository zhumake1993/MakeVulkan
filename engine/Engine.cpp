#include "Engine.h"
#include "Tools.h"
#include "Example.h"
#include "GfxDevice.h"
#include "DeviceProperties.h"
#include "InputManager.h"

Engine::Engine(Example* example) :
	m_Example(example)
{
}

Engine::~Engine()
{
}

void Engine::Init()
{
	// 设置DeviceProperties，初始化GfxDevice会用到
	CreateDeviceProperties();
	m_Example->ConfigDeviceProperties();

	// 初始化GfxDevice
	CreateGfxDevice();

	// 初始化Manager
	

	// 初始化Engine
	//todo

	// 初始化Example
	m_Example->Init();
}

void Engine::Release()
{
	// 等待GfxDevice
	GetGfxDevice().DeviceWaitIdle();

	// 清理Example
	m_Example->Release();
	RELEASE(m_Example);

	// 清理Engine
	//todo

	// 清理Manager
	

	// 清理GfxDevice
	ReleaseGfxDevice();

	// 清理DeviceProperties
	ReleaseDeviceProperties();
}

void Engine::Update()
{
	// 顺序很重要！！！

	// 更新时间
	

	//PROFILER(Engine_TickEngine);

	auto& device = GetGfxDevice();
	device.WaitForPresent();
	device.AcquireNextImage();

	// 更新Example
	m_Example->Update();

	// 必须在游戏逻辑更新完之后再更新输入
	inputManager.Tick();

	// 更新UI逻辑
	//m_Imgui->Prepare();
	//TickUI();
	//ImGui::Render();
	

	// 更新UI顶点计算
	//m_Imgui->Tick();

	// Present
	device.QueueSubmit();
	device.QueuePresent();
	device.Update();
}