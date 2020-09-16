#pragma once

#include "Common.h"
#include "NonCopyable.h"

class Engine;

class Application : public NonCopyable
{

public:

	Application(Engine* engine);
	virtual ~Application();

	void CleanUp();
	void Init();
	void Run();
	void DeActivate();

#if defined(VK_USE_PLATFORM_ANDROID_KHR)

	void GainFocus();
	void LostFocus();

#endif

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
	
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	// true if application has focused, false if moved to background
	bool m_Focused = false;
#endif
};

extern Application* application;

#if defined(_WIN32)
void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
int32_t HandleAppInput(struct android_app* app, AInputEvent* event);
void HandleAppCommand(android_app* app, int32_t cmd);
#endif