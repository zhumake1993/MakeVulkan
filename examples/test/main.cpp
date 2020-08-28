#include "Common.h"
#include "Application.h"
#include "triangle.h"

#ifdef _WIN32

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	global::windowInstance = hInstance;

	ConfigGlobalSettings();

	application = new Application();
	application->Init(new Triangle());
	application->Run();
	application->CleanUp();

	system("PAUSE");

	delete(application);
	return 0;
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void android_main(android_app* state)
{
	ConfigGlobalSettings();

	vulkanExample = new VulkanExample();

	androidApp = state;
	androidApp->userData = vulkanExample;
	androidApp->onAppCmd = VulkanExample::HandleAppCommand;
	androidApp->onInputEvent = VulkanExample::HandleAppInput;
	vulkanExample->Run();

	delete(vulkanExample);
}

#endif