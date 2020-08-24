#pragma once

#include "VulkanCommon.h"

class Application
{

public:

	Application();
	virtual ~Application();

#if defined(_WIN32)
	void SetupConsole();
	HWND SetupWindow(HINSTANCE hinstance, WNDPROC wndproc);
	void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	void HandleTouchScreenEvent(int32_t action, AInputEvent* event);
	static int32_t HandleAppInput(struct android_app* app, AInputEvent* event);
	static void HandleAppCommand(android_app* app, int32_t cmd);
#endif

	virtual void CleanUp() = 0;
	virtual void Init() = 0;
	virtual void Prepare() = 0;
	void Run();

protected:

	virtual void KeyPress(uint32_t) {}
	virtual void KeyUp(uint32_t) {}

private:

	virtual void Draw() {}

protected:

	bool m_CanRender = false;

#if defined(_WIN32)
	HINSTANCE m_WindowInstance;
	HWND m_WindowHandle;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	bool m_Focused = false;
	std::string androidProduct;
#endif
	
	KeyboardInput m_KeyboardInput;
};