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
};

extern Application* application;

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
int32_t HandleAppInput(struct android_app* app, AInputEvent* event);
void HandleAppCommand(android_app* app, int32_t cmd);
#endif