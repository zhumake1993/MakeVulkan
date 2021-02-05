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
	// ����DeviceProperties����ʼ��GfxDevice���õ�
	CreateDeviceProperties();
	m_Example->ConfigDeviceProperties();

	// ��ʼ��GfxDevice
	CreateGfxDevice();

	// ��ʼ��Manager
	

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
	

	// ����GfxDevice
	ReleaseGfxDevice();

	// ����DeviceProperties
	ReleaseDeviceProperties();
}

void Engine::Update()
{
	// ˳�����Ҫ������

	// ����ʱ��
	

	//PROFILER(Engine_TickEngine);

	auto& device = GetGfxDevice();
	device.WaitForPresent();
	device.AcquireNextImage();

	// ����Example
	m_Example->Update();

	// ��������Ϸ�߼�������֮���ٸ�������
	inputManager.Tick();

	// ����UI�߼�
	//m_Imgui->Prepare();
	//TickUI();
	//ImGui::Render();
	

	// ����UI�������
	//m_Imgui->Tick();

	// Present
	device.QueueSubmit();
	device.QueuePresent();
	device.Update();
}