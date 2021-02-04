#pragma once

#include "Env.h"
#include "NonCopyable.h"

class Engine;
class Example;

class Application : public NonCopyable
{
public:

	Application(Example* example);
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

extern Application* application;

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
int32_t HandleAppInput(struct android_app* app, AInputEvent* event);
void HandleAppCommand(android_app* app, int32_t cmd);
#endif