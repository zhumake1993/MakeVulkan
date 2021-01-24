#include "Engine.h"
#include "Tools.h"
#include "Example.h"
#include "GfxDevice.h"

Engine::Engine(Example* example) :
	m_Example(example)
{
}

Engine::~Engine()
{
}

void Engine::Init()
{
	// 初始化GfxDevice
	GetGfxDevice().Init();

	// 初始化Manager
	//todo

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
	//todo

	// 清理GfxDevice
	GetGfxDevice().Release();
}

void Engine::Update()
{
	// 顺序很重要！！！

	// 更新时间
	//auto& timeMgr = GetTimeMgr();
	//timeMgr.Tick();

	//PROFILER(Engine_TickEngine);

	// 更新Example
	m_Example->Update();

	// 更新UI逻辑
	//m_Imgui->Prepare();
	//TickUI();
	//ImGui::Render();

	// 更新输入
	//input.Tick();

	// 更新UI顶点计算
	//m_Imgui->Tick();

	// 更新GfxDevice
	GetGfxDevice().Update();
}