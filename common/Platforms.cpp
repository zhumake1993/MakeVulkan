#include "Platforms.h"

#ifdef _WIN32

HINSTANCE gWindowInstance = nullptr;
HWND gWindowHandle = nullptr;

void platform::SetWindowInstance(HINSTANCE windowInstance)
{
	gWindowInstance = windowInstance;
}

HINSTANCE platform::GetWindowInstance()
{
	return gWindowInstance;
}

void platform::SetWindowHandle(HWND windowHandle)
{
	gWindowHandle = windowHandle;
}

HWND platform::GetWindowHandle()
{
	return gWindowHandle;
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

android_app* gAndroidApp = nullptr;

void platform::SetAndroidApp(android_app * androidApp)
{
	gAndroidApp = androidApp;
}

android_app * platform::GetAndroidApp()
{
	return gAndroidApp;
}

#endif