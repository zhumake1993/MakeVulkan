#include "Engine.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "ProfilerManager.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "RendererScene.h"
#include "Imgui.h"
#include "ResourceManager.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::InitEngine()
{
	PreInit();

	// ��ʼ��GfxDevice
	CreateGfxDevice();

	// ��ʼ��Manager
	CreateProfilerManager();

	m_TimeManager = new TimeManager();
	m_RendererScene = new RendererScene();
	m_Imgui = new Imgui();

	// ��ʼ������
	Init();
}

void Engine::ReleaseEngine()
{
	// �ȴ�GfxDevice
	GetGfxDevice().DeviceWaitIdle();

	// ��������
	Release();

	// ����Manager
	GetProfilerManager().WriteToFile();
	ReleaseProfilerManager();

	RELEASE(m_TimeManager);
	RELEASE(m_RendererScene);
	RELEASE(m_Imgui);

	// ����GfxDevice
	ReleaseGfxDevice();
}

void Engine::UpdateEngine()
{
	auto& device = GetGfxDevice();

	// �ȸ���ProfilerManager
	GetProfilerManager().Update();

	m_TimeManager->Update();
	m_Imgui->Prepare(m_TimeManager->GetDeltaTime());

	PROFILER(Engine_Update);

	// ��������
	Update();

	// ��������Ϸ�߼�������֮���ٸ�������
	GetInputManager().Tick();

	// �ȴ�Fence
	device.WaitForPresent();
	device.AcquireNextImage();

	// ���ύ��֡��ָ��֮ǰresolve������֡д���ʱ������ܻ��֮ǰ�ĸ���
	device.ResolveTimeStamp();

	// �ύ��Ⱦָ��
	Draw();

	// Present
	device.QueueSubmit();
	device.QueuePresent();

	// todo
	GetResourceManager().ReleaseTempAttachment();

	device.Update();
}