#pragma once

#include "NonCopyable.h"

class Engine;

class Application : public NonCopyable
{
public:

	Application(Engine* engine);
	virtual ~Application();

	void Init();
	void Release();
	void Run();

	void Activate();
	void DeActivate();

private:

#if defined(_WIN32)

	// ����console
	void SetupConsole();

	// ���ô���
	void SetupWindow();

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	// ��ʾ��׿�豸�����ƺ�������
	void DisplayAndroidProduct();

	// �ڰ�׿�豸��Vulkan��Ҫ��̬����
	void LoadVulkan();

#endif

private:

	Engine* m_Engine;
	bool m_CanRender = false;
};

void SetApplication(Application* application);