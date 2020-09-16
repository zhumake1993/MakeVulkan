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

	// 设置console
	void SetupConsole();

	// 设置窗口
	void SetupWindow();

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	// 显示安卓设备的名称和制造商
	void DisplayAndroidProduct();

	// 在安卓设备上Vulkan需要动态载入
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