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
	// ����DeviceProperties����ʼ��GfxDevice���õ�
	CreateDeviceProperties();
	m_Example->ConfigDeviceProperties();

	// ��ʼ��GfxDevice
	CreateGfxDevice();

	// ��ʼ��Manager
	CreateProfilerManager();

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

	// ����Manager
	GetProfilerManager().WriteToFile();
	ReleaseProfilerManager();

	// ����GfxDevice
	ReleaseGfxDevice();

	// ����DeviceProperties
	ReleaseDeviceProperties();
}

void Engine::Update()
{
	auto& device = GetGfxDevice();

	// �ȸ���ProfilerManager
	GetProfilerManager().Update();

	PROFILER(Engine_Update);

	// �����߼�
	m_Example->Update();

	// ��������Ϸ�߼�������֮���ٸ�������
	inputManager.Tick();

	// �ȴ�Fence
	device.WaitForPresent();
	device.AcquireNextImage();

	// ���ύ��֡��ָ��֮ǰresolve������֡д���ʱ������ܻ��֮ǰ�ĸ���
	device.ResolveTimeStamp();

	// �ύ��Ⱦָ��
	m_Example->Draw();

	// Present
	device.QueueSubmit();
	device.QueuePresent();
	device.Update();
}