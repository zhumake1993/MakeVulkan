#include "Env.h"

#ifdef _WIN32

HINSTANCE windowInstance;
HWND windowHandle;

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

android_app* androidApp;

#endif