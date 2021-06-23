#pragma once

#ifdef _WIN32

#include <windows.h>

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

#include <android/log.h>
#include <android_native_app_glue.h>
#include <sys/system_properties.h>
#include <android/asset_manager.h>

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

	int32_t HandleAppInput(struct android_app* app, AInputEvent* event);
	void HandleAppCommand(android_app* app, int32_t cmd);

#endif
}


// here?
#ifdef _WIN32

#define MAIN(name)																						\
																										\
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)		\
{																										\
	windowInstance = hInstance;																			\
																										\
	application = new Application(new name());															\
	application->Init();																				\
	application->Run();																					\
	application->Release();																				\
	RELEASE(application);																				\
																										\
	LOG("Application exits.");																			\
	system("PAUSE");																					\
	return 0;																							\
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

#define MAIN(name)																						\
																										\
void android_main(android_app* state)																	\
{																										\
	androidApp = state;																					\
																										\
	application = new Application(new name());															\
	state->userData = application;																		\
	state->onAppCmd = HandleAppCommand;																	\
	state->onInputEvent = HandleAppInput;																\
	application->Run();																					\
	application->Release();																				\
	RELEASE(application);																				\
																										\
	LOG("Application exits.");																			\
}

#endif