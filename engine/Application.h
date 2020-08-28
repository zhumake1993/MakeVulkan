#pragma once

#include "Common.h"

class Engine;

class Application
{

public:

	Application();
	virtual ~Application();

	void CleanUp();
	void Init(Engine* engine);
	void Run();
	void Close();

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

extern Application* application;