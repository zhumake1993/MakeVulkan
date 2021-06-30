#pragma once

#ifdef _WIN32

#include <windows.h>

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

#include <android_native_app_glue.h>
#include <sys/system_properties.h>

#endif

namespace platform
{

#ifdef _WIN32

	void SetWindowInstance(HINSTANCE windowInstance);
	HINSTANCE GetWindowInstance();

	void SetWindowHandle(HWND windowHandle);
	HWND GetWindowHandle();

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	void SetAndroidApp(android_app* androidApp);
	android_app* GetAndroidApp();

#endif
}