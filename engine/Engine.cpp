#include "Engine.h"
#include "GfxDevice.h"
#include "ProfilerManager.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Imgui.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::InitEngine()
{
	PreInit();

	// 初始化GfxDevice
	CreateGfxDevice();

	// 初始化Manager
	CreateProfilerManager();

	m_TimeManager = new TimeManager();
	m_Imgui = new Imgui();

	// 初始化子类
	Init();
}

void Engine::ReleaseEngine()
{
	// 等待GfxDevice
	GetGfxDevice().DeviceWaitIdle();

	// 清理子类
	Release();

	// 清理Manager
	GetProfilerManager().WriteToFile();
	ReleaseProfilerManager();

	delete m_TimeManager;
	delete m_Imgui;

	// 清理GfxDevice
	ReleaseGfxDevice();
}

void Engine::UpdateEngine()
{
	auto& device = GetGfxDevice();

	// 先更新ProfilerManager
	GetProfilerManager().Update();

	m_TimeManager->Update();
	m_Imgui->Prepare(m_TimeManager->GetDeltaTime());

	PROFILER(Engine_Update);

	// 更新子类
	Update();

	// 必须在游戏逻辑更新完之后再更新输入
	inputManager.Tick();

	// 等待Fence
	device.WaitForPresent();
	device.AcquireNextImage();

	// 在提交本帧的指令之前resolve，否则本帧写入的时间戳可能会把之前的覆盖
	device.ResolveTimeStamp();

	// 提交渲染指令
	Draw();

	// Present
	device.QueueSubmit();
	device.QueuePresent();

	UpdateAfterDraw();

	device.Update();
}