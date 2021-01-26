#include "Engine.h"
#include "Tools.h"
#include "Example.h"
#include "GfxDevice.h"
#include "DeviceProperties.h"

Engine::Engine(Example* example) :
	m_Example(example)
{
}

Engine::~Engine()
{
}

void Engine::Init()
{
	// ����DeviceProperties����ʼ��GfxDevice���õ�
	CreateDeviceProperties();
	m_Example->ConfigDeviceProperties();

	// ��ʼ��GfxDevice
	CreateGfxDevice();

	// ��ʼ��Manager
	//todo

	// ��ʼ��Engine
	//todo

	// ��ʼ��Example
	m_Example->Init();
}

void Engine::Release()
{
	// �ȴ�GfxDevice
	GetGfxDevice().DeviceWaitIdle();

	// ����Example
	m_Example->Release();
	RELEASE(m_Example);

	// ����Engine
	//todo

	// ����Manager
	//todo

	// ����GfxDevice
	ReleaseGfxDevice();

	// ����DeviceProperties
	ReleaseDeviceProperties();
}

void Engine::Update()
{
	// ˳�����Ҫ������

	// ����ʱ��
	//auto& timeMgr = GetTimeMgr();
	//timeMgr.Tick();

	//PROFILER(Engine_TickEngine);

	auto& device = GetGfxDevice();
	device.WaitForPresent();
	device.AcquireNextImage();

	// ����Example
	m_Example->Update();

	// ����UI�߼�
	//m_Imgui->Prepare();
	//TickUI();
	//ImGui::Render();

	// ��������
	//input.Tick();

	// ����UI�������
	//m_Imgui->Tick();

	// Present
	device.QueueSubmit();
	device.QueuePresent();
	device.Update();
}