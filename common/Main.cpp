#include "Env.h"
#include "Log.h"
#include "Tools.h"
#include "Application.h"

#ifdef _WIN32

#if USE_VISUAL_LEAK_DETECTOR
#include "vld.h"
#endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	windowInstance = hInstance;

	application = new Application();
	application->Init();
	application->Run();
	application->Release();
	RELEASE(application);

	LOG("Application exits.");
	system("PAUSE");
	return 0;
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void android_main(android_app* state)
{
	androidApp = state;

	application = new Application();
	state->userData = application;
	state->onAppCmd = HandleAppCommand;
	state->onInputEvent = HandleAppInput;
	application->Run();
	application->Release();
	RELEASE(application);

	LOG("Application exits.");
}

#endif