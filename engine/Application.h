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