#include "Application.h"
#include "Settings.h"
#include "Log.h"
#include "Engine.h"
#include "Tools.h"
#include "Input.h"

Application* application;

Application::Application(Example* example)
{
	m_Engine = new Engine(example);
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

	m_Engine->Init();
	m_CanRender = true;
}

void Application::Release()
{
	m_Engine->Release();
	RELEASE(m_Engine);
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
		if (m_CanRender && !IsIconic(windowHandle)) {
			m_Engine->Update();
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
	SetConsoleTitle(TEXT(consoleTitle.c_str()));
}

void Application::SetupWindow()
{
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = windowInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = windowClassName.c_str();
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
	windowRect.right = windowWidth;
	windowRect.bottom = windowHeight;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	windowHandle = CreateWindowEx(0,
		windowClassName.c_str(),
		windowTitleName.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		windowInstance,
		NULL);

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
	switch (uMsg)
	{
	case WM_CLOSE:
		application->DeActivate();
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(windowHandle, NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case KEY_W:
			input.key_W = true;
			break;
		case KEY_S:
			input.key_S = true;
			break;
		case KEY_A:
			input.key_A = true;
			break;
		case KEY_D:
			input.key_D = true;
			break;
		case KEY_Q:
			input.key_Q = true;
			break;
		case KEY_E:
			input.key_E = true;
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case KEY_W:
			input.key_W = false;
			break;
		case KEY_S:
			input.key_S = false;
			break;
		case KEY_A:
			input.key_A = false;
			break;
		case KEY_D:
			input.key_D = false;
			break;
		case KEY_Q:
			input.key_Q = false;
			break;
		case KEY_E:
			input.key_E = false;
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		input.key_MouseLeft = true;
		input.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		input.key_MouseRight = true;
		input.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_MBUTTONDOWN:
		// 滚轮
		break;
	case WM_LBUTTONUP:
		input.key_MouseLeft = false;
		input.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		input.key_MouseRight = false;
		input.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
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
		input.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
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
		input.count = 0;
		break;
	}
	case AMOTION_EVENT_ACTION_DOWN:
	{
		input.count = 1;

		input.pos0.x = AMotionEvent_getX(event, 0);
		input.pos0.y = AMotionEvent_getY(event, 0);
		input.oldPos0.x = AMotionEvent_getX(event, 0);
		input.oldPos0.y = AMotionEvent_getY(event, 0);

		break;
	}
	case AMOTION_EVENT_ACTION_MOVE:
	{
		if (AMotionEvent_getPointerCount(event) == 1) {
			input.pos0.x = AMotionEvent_getX(event, 0);
			input.pos0.y = AMotionEvent_getY(event, 0);

			// 
			if (input.count == 2) {
				input.count = 1;

				input.oldPos0.x = AMotionEvent_getX(event, 0);
				input.oldPos0.y = AMotionEvent_getY(event, 0);
			}
		}
		else if (AMotionEvent_getPointerCount(event) == 2) {

			input.pos0.x = AMotionEvent_getX(event, 0);
			input.pos0.y = AMotionEvent_getY(event, 0);

			input.pos1.x = AMotionEvent_getX(event, 1);
			input.pos1.y = AMotionEvent_getY(event, 1);

			// 
			if (input.count == 1) {
				input.count = 2;

				input.oldPos0.x = AMotionEvent_getX(event, 0);
				input.oldPos0.y = AMotionEvent_getY(event, 0);

				input.oldPos1.x = AMotionEvent_getX(event, 1);
				input.oldPos1.y = AMotionEvent_getY(event, 1);
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

