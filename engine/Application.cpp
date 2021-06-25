#include "Application.h"
#include "Platforms.h"
#include "GlobalSettings.h"
#include "Log.h"
#include "Engine.h"
#include "InputManager.h"
#include <stdio.h>

Application* gApplication = nullptr;

void SetApplication(Application * application)
{
	gApplication = application;
}

Application::Application(Engine* engine)
{
	m_Engine = engine;
}

Application::~Application()
{
}

void Application::Init()
{
#if defined(_WIN32)

	// VLD跟控制台输出有冲突
#if !(USE_VISUAL_LEAK_DETECTOR)
	SetupConsole();
#endif // !USE_VISUAL_LEAK_DETECTOR

	SetupWindow();

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	DisplayAndroidProduct();
	LoadVulkan();

#endif

	m_Engine->InitEngine();
	m_CanRender = true;
}

void Application::Release()
{
	m_Engine->ReleaseEngine();
	delete m_Engine;
}

void Application::Run()
{
#if defined(_WIN32)
	MSG msg;
	bool quitMessageReceived = false;
	while (!quitMessageReceived) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				quitMessageReceived = true;
				break;
			}
		}
		if (m_CanRender && !IsIconic(platform::GetWindowHandle())) {
			m_Engine->UpdateEngine();
		}
	}
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	while (1)
	{
		int events;
		struct android_poll_source* source;
		bool destroy = false;

		bool focused = true;

		while (ALooper_pollAll(focused ? 0 : -1, NULL, &events, (void**)&source) >= 0)
		{
			if (source != NULL)
			{
				source->process(androidApp, source);
			}
			if (androidApp->destroyRequested != 0)
			{
				LOG("Android app destroy requested");
				destroy = true;
				break;
			}
		}

		if (destroy)
		{
			ANativeActivity_finish(androidApp->activity);
			break;
		}

		if (m_CanRender) {
			m_Engine->Update();
		}
	}
#endif
}

void Application::Activate()
{
	m_CanRender = true;
}

void Application::DeActivate()
{
	m_CanRender = false;
}

#if defined(_WIN32)

void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HandleMessages(hWnd, uMsg, wParam, lParam);
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void Application::SetupConsole()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w+", stdout);
	freopen_s(&stream, "CONOUT$", "w+", stderr);
	SetConsoleTitle(TEXT(GetGlobalSettings().consoleTitle.c_str()));
}

void Application::SetupWindow()
{
	auto& gs = GetGlobalSettings();

	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = platform::GetWindowInstance();
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = gs.windowClassName.c_str();
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		LOG("Could not register window class!");
		fflush(stdout);
		exit(1);
	}

	DWORD dwExStyle;
	DWORD dwStyle;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = gs.windowWidth;
	windowRect.bottom = gs.windowHeight;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	HWND windowHandle = CreateWindowEx(0,
		gs.windowClassName.c_str(),
		gs.windowTitleName.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		platform::GetWindowInstance(),
		NULL);

	platform::SetWindowHandle(windowHandle);

	if (!windowHandle)
	{
		LOG("Could not create window!");
		fflush(stdout);
		return;
		exit(1);
	}

	// 居中
	uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
	uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
	SetWindowPos(windowHandle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	ShowWindow(windowHandle, SW_SHOW);
	SetForegroundWindow(windowHandle);
	SetFocus(windowHandle);

	LOG("SetupWindow success\n");
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void Application::DisplayAndroidProduct()
{
	std::string androidProduct = "";
	char prop[PROP_VALUE_MAX + 1];
	int len = __system_property_get("ro.product.manufacturer", prop);
	if (len > 0) {
		androidProduct += std::string(prop) + " ";
	};
	len = __system_property_get("ro.product.model", prop);
	if (len > 0) {
		androidProduct += std::string(prop);
	};
	LOG("androidProduct = %s", androidProduct.c_str());
}

void Application::LoadVulkan()
{
	if (!InitVulkan())
	{
		LOG("Vulkan is unavailable, install vulkan and re-start");
		EXIT;
	}
	LOG("Vulkan Ready");
}

#endif

#if defined(_WIN32)

void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto& inputManager = GetInputManager();

	switch (uMsg)
	{
	case WM_CLOSE:
		gApplication->DeActivate();
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(platform::GetWindowHandle(), NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case KEY_W:
			inputManager.key_W = true;
			break;
		case KEY_S:
			inputManager.key_S = true;
			break;
		case KEY_A:
			inputManager.key_A = true;
			break;
		case KEY_D:
			inputManager.key_D = true;
			break;
		case KEY_Q:
			inputManager.key_Q = true;
			break;
		case KEY_E:
			inputManager.key_E = true;
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case KEY_W:
			inputManager.key_W = false;
			break;
		case KEY_S:
			inputManager.key_S = false;
			break;
		case KEY_A:
			inputManager.key_A = false;
			break;
		case KEY_D:
			inputManager.key_D = false;
			break;
		case KEY_Q:
			inputManager.key_Q = false;
			break;
		case KEY_E:
			inputManager.key_E = false;
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		inputManager.key_MouseLeft = true;
		inputManager.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		inputManager.key_MouseRight = true;
		inputManager.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_MBUTTONDOWN:
		// 滚轮
		break;
	case WM_LBUTTONUP:
		inputManager.key_MouseLeft = false;
		inputManager.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		inputManager.key_MouseRight = false;
		inputManager.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_MBUTTONUP:
		// 滚轮
		break;
	case WM_MOUSEWHEEL:
	{
		// 滚轮
		break;
	}
	case WM_MOUSEMOVE:
	{
		inputManager.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	}
	case WM_SIZE:
		//
		break;
	case WM_GETMINMAXINFO:
	{
		//
		break;
	}
	case WM_ENTERSIZEMOVE:
		//
		break;
	case WM_EXITSIZEMOVE:
		//
		break;
	}
}
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void HandleTouchScreenEvent(int32_t action, AInputEvent* event)
{
	switch (action)
	{
	case AMOTION_EVENT_ACTION_UP:
	{
		inputManager.count = 0;
		break;
	}
	case AMOTION_EVENT_ACTION_DOWN:
	{
		inputManager.count = 1;

		inputManager.pos0.x = AMotionEvent_getX(event, 0);
		inputManager.pos0.y = AMotionEvent_getY(event, 0);
		inputManager.oldPos0.x = AMotionEvent_getX(event, 0);
		inputManager.oldPos0.y = AMotionEvent_getY(event, 0);

		break;
	}
	case AMOTION_EVENT_ACTION_MOVE:
	{
		if (AMotionEvent_getPointerCount(event) == 1) {
			inputManager.pos0.x = AMotionEvent_getX(event, 0);
			inputManager.pos0.y = AMotionEvent_getY(event, 0);

			// 
			if (inputManager.count == 2) {
				inputManager.count = 1;

				inputManager.oldPos0.x = AMotionEvent_getX(event, 0);
				inputManager.oldPos0.y = AMotionEvent_getY(event, 0);
			}
		}
		else if (AMotionEvent_getPointerCount(event) == 2) {

			inputManager.pos0.x = AMotionEvent_getX(event, 0);
			inputManager.pos0.y = AMotionEvent_getY(event, 0);

			inputManager.pos1.x = AMotionEvent_getX(event, 1);
			inputManager.pos1.y = AMotionEvent_getY(event, 1);

			// 
			if (inputManager.count == 1) {
				inputManager.count = 2;

				inputManager.oldPos0.x = AMotionEvent_getX(event, 0);
				inputManager.oldPos0.y = AMotionEvent_getY(event, 0);

				inputManager.oldPos1.x = AMotionEvent_getX(event, 1);
				inputManager.oldPos1.y = AMotionEvent_getY(event, 1);
			}
		}
		else {
			// 不处理2个触点以上的情况
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

int32_t HandleAppInput(struct android_app* app, AInputEvent* event)
{
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		int32_t eventSource = AInputEvent_getSource(event);
		switch (eventSource)
		{
		case AINPUT_SOURCE_JOYSTICK:
		{
			// 不处理手柄事件
			LOG("Joystick event not handled!");
			return 0;
		}

		case AINPUT_SOURCE_TOUCHSCREEN:
		{
			int32_t action = AMotionEvent_getAction(event);
			HandleTouchScreenEvent(action, event);
		}

		return 1;
		}
	}

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		// 不处理键盘事件
		LOG("Key event not handled!");
		return 0;
	}

	return 0;
}

void HandleAppCommand(android_app * app, int32_t cmd)
{
	switch (cmd)
	{
	case APP_CMD_SAVE_STATE:
		LOG("APP_CMD_SAVE_STATE");
		break;
	case APP_CMD_INIT_WINDOW:
		LOG("APP_CMD_INIT_WINDOW");
		application->Init();
		break;
	case APP_CMD_LOST_FOCUS:
		LOG("APP_CMD_LOST_FOCUS");
		application->DeActivate();
		break;
	case APP_CMD_GAINED_FOCUS:
		LOG("APP_CMD_GAINED_FOCUS");
		application->Activate();
		break;
	case APP_CMD_TERM_WINDOW:
		LOG("APP_CMD_TERM_WINDOW");
		application->Release();
		break;
	}
}

#endif