#include "Engine.h"
#include "Tools.h"
#include "Example.h"
#include "GfxDevice.h"
#include "DeviceProperties.h"
#include "InputManager.h"
#include "ProfilerManager.h"

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
	CreateProfilerManager();

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

	// 清理Manager
	GetProfilerManager().WriteToFile();
	ReleaseProfilerManager();

	// 清理GfxDevice
	ReleaseGfxDevice();

	// 清理DeviceProperties
	ReleaseDeviceProperties();
}

void Engine::Update()
{
	auto& device = GetGfxDevice();

	// 先更新ProfilerManager
	GetProfilerManager().Update();

	PROFILER(Engine_Update);

	// 更新逻辑
	m_Example->Update();

	// 必须在游戏逻辑更新完之后再更新输入
	inputManager.Tick();

	// 等待Fence
	device.WaitForPresent();
	device.AcquireNextImage();

	// 在提交本帧的指令之前resolve，否则本帧写入的时间戳可能会把之前的覆盖
	device.ResolveTimeStamp();

	// 提交渲染指令
	m_Example->Draw();

	// Present
	device.QueueSubmit();
	device.QueuePresent();
	device.Update();
}