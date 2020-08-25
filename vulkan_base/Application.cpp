#include "Application.h"
#include <iostream>
#include "Tools.h"

Application::Application()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	// Get Android device name and manufacturer (to display along GPU name)
	androidProduct = "";
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
#endif

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	if (!InitVulkan())
	{
		LOG("Vulkan is unavailable, install vulkan and re-start");
		assert(false);
	}
	LOG("Vulkan Ready");
#endif
}

Application::~Application()
{
}

#if defined(_WIN32)

void Application::SetupConsole()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w+", stdout);
	freopen_s(&stream, "CONOUT$", "w+", stderr);
	SetConsoleTitle(TEXT(global::consoleTitle.c_str()));
}

HWND Application::SetupWindow(HINSTANCE hinstance, WNDPROC wndproc)
{
	m_WindowInstance = hinstance;

	WNDCLASSEX wndClass;

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = wndproc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hinstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = global::windowClassName.c_str();
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		std::cout << "Could not register window class!\n";
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
	windowRect.right = global::windowWidth;
	windowRect.bottom = global::windowHeight;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	m_WindowHandle = CreateWindowEx(0,
		global::windowClassName.c_str(),
		global::windowTitleName.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hinstance,
		NULL);

	if (!m_WindowHandle)
	{
		std::cout << "Could not create window!\n";
		fflush(stdout);
		return nullptr;
		exit(1);
	}

	// 居中
	uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
	uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
	SetWindowPos(m_WindowHandle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	ShowWindow(m_WindowHandle, SW_SHOW);
	SetForegroundWindow(m_WindowHandle);
	SetFocus(m_WindowHandle);

	return m_WindowHandle;
}

void Application::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		m_CanRender = false;
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(m_WindowHandle, NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case KEY_W:
			m_KeyboardInput.key_W = true;
			break;
		case KEY_S:
			m_KeyboardInput.key_S = true;
			break;
		case KEY_A:
			m_KeyboardInput.key_A = true;
			break;
		case KEY_D:
			m_KeyboardInput.key_D = true;
			break;
		case KEY_Q:
			m_KeyboardInput.key_Q = true;
			break;
		case KEY_E:
			m_KeyboardInput.key_E = true;
			break;
		}
		KeyPress((uint32_t)wParam);
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case KEY_W:
			m_KeyboardInput.key_W = false;
			break;
		case KEY_S:
			m_KeyboardInput.key_S = false;
			break;
		case KEY_A:
			m_KeyboardInput.key_A = false;
			break;
		case KEY_D:
			m_KeyboardInput.key_D = false;
			break;
		case KEY_Q:
			m_KeyboardInput.key_Q = false;
			break;
		case KEY_E:
			m_KeyboardInput.key_E = false;
			break;
		}
		KeyUp((uint32_t)wParam);
		break;
	case WM_LBUTTONDOWN:
		m_KeyboardInput.key_MouseLeft = true;
		m_KeyboardInput.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		m_KeyboardInput.key_MouseRight = true;
		m_KeyboardInput.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_MBUTTONDOWN:
		// 滚轮
		break;
	case WM_LBUTTONUP:
		m_KeyboardInput.key_MouseLeft = false;
		m_KeyboardInput.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		m_KeyboardInput.key_MouseRight = false;
		m_KeyboardInput.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
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
		m_KeyboardInput.pos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;
	}
	/*case WM_SIZE:
		if ((prepared) && (wParam != SIZE_MINIMIZED))
		{
			if ((resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED)))
			{
				destWidth = LOWORD(lParam);
				destHeight = HIWORD(lParam);
				windowResize();
			}
		}
		break;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO minMaxInfo = (LPMINMAXINFO)lParam;
		minMaxInfo->ptMinTrackSize.x = 64;
		minMaxInfo->ptMinTrackSize.y = 64;
		break;
	}
	case WM_ENTERSIZEMOVE:
		resizing = true;
		break;
	case WM_EXITSIZEMOVE:
		resizing = false;
		break;*/
	}
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void Application::HandleTouchScreenEvent(int32_t action, AInputEvent* event)
{
	switch (action)
	{
		case AMOTION_EVENT_ACTION_UP:
		{
			m_KeyboardInput.count = 0;
			break;
		}
		case AMOTION_EVENT_ACTION_DOWN:
		{
			m_KeyboardInput.count = 1;

			m_KeyboardInput.pos0.x = AMotionEvent_getX(event, 0);
			m_KeyboardInput.pos0.y = AMotionEvent_getY(event, 0);
			m_KeyboardInput.oldPos0.x = AMotionEvent_getX(event, 0);
			m_KeyboardInput.oldPos0.y = AMotionEvent_getY(event, 0);

			break;
		}
		case AMOTION_EVENT_ACTION_MOVE:
		{
			if (AMotionEvent_getPointerCount(event) == 1) {
				m_KeyboardInput.pos0.x = AMotionEvent_getX(event, 0);
				m_KeyboardInput.pos0.y = AMotionEvent_getY(event, 0);

				// 
				if (m_KeyboardInput.count == 2) {
					m_KeyboardInput.count = 1;

					m_KeyboardInput.oldPos0.x = AMotionEvent_getX(event, 0);
					m_KeyboardInput.oldPos0.y = AMotionEvent_getY(event, 0);
				}
			}
			else if (AMotionEvent_getPointerCount(event) == 2) {

				m_KeyboardInput.pos0.x = AMotionEvent_getX(event, 0);
				m_KeyboardInput.pos0.y = AMotionEvent_getY(event, 0);

				m_KeyboardInput.pos1.x = AMotionEvent_getX(event, 1);
				m_KeyboardInput.pos1.y = AMotionEvent_getY(event, 1);

				// 
				if (m_KeyboardInput.count == 1) {
					m_KeyboardInput.count = 2;

					m_KeyboardInput.oldPos0.x = AMotionEvent_getX(event, 0);
					m_KeyboardInput.oldPos0.y = AMotionEvent_getY(event, 0);

					m_KeyboardInput.oldPos1.x = AMotionEvent_getX(event, 1);
					m_KeyboardInput.oldPos1.y = AMotionEvent_getY(event, 1);
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

int32_t Application::HandleAppInput(struct android_app* app, AInputEvent* event)
{
	Application* application = reinterpret_cast<Application*>(app->userData);
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
				application->HandleTouchScreenEvent(action, event);
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

void Application::HandleAppCommand(android_app * app, int32_t cmd)
{
	Application* application = reinterpret_cast<Application*>(app->userData);
	switch (cmd)
	{
	case APP_CMD_SAVE_STATE:
		LOG("APP_CMD_SAVE_STATE");
		break;
	case APP_CMD_INIT_WINDOW:
		LOG("APP_CMD_INIT_WINDOW");
		application->Init();
		application->Prepare();
		break;
	case APP_CMD_LOST_FOCUS:
		LOG("APP_CMD_LOST_FOCUS");
		application->m_Focused = false;
		break;
	case APP_CMD_GAINED_FOCUS:
		LOG("APP_CMD_GAINED_FOCUS");
		application->m_Focused = true;
		break;
	case APP_CMD_TERM_WINDOW:
		LOG("APP_CMD_TERM_WINDOW");
		application->CleanUp();
		break;
	}
}

#endif

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
		if (m_CanRender && !IsIconic(m_WindowHandle)) {
			Draw();
		}
	}
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	while (1)
	{
		int events;
		struct android_poll_source* source;
		bool destroy = false;

		m_Focused = true;

		while (ALooper_pollAll(m_Focused ? 0 : -1, NULL, &events, (void**)&source) >= 0)
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

		if (m_CanRender)
		{
			Draw();
		}
	}
#endif
}