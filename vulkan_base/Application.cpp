#include "Application.h"
#include <iostream>
#include "Tools.h"

Application::Application()
{
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
		//case WM_KEYDOWN:
		//	switch (wParam)
		//	{
		//	case KEY_P:
		//		paused = !paused;
		//		break;
		//	case KEY_F1:
		//		if (settings.overlay) {
		//			UIOverlay.visible = !UIOverlay.visible;
		//		}
		//		break;
		//	case KEY_ESCAPE:
		//		PostQuitMessage(0);
		//		break;
		//	}

		//	if (camera.firstperson)
		//	{
		//		switch (wParam)
		//		{
		//		case KEY_W:
		//			camera.keys.up = true;
		//			break;
		//		case KEY_S:
		//			camera.keys.down = true;
		//			break;
		//		case KEY_A:
		//			camera.keys.left = true;
		//			break;
		//		case KEY_D:
		//			camera.keys.right = true;
		//			break;
		//		}
		//	}

		//	keyPressed((uint32_t)wParam);
		//	break;
		//case WM_KEYUP:
		//	if (camera.firstperson)
		//	{
		//		switch (wParam)
		//		{
		//		case KEY_W:
		//			camera.keys.up = false;
		//			break;
		//		case KEY_S:
		//			camera.keys.down = false;
		//			break;
		//		case KEY_A:
		//			camera.keys.left = false;
		//			break;
		//		case KEY_D:
		//			camera.keys.right = false;
		//			break;
		//		}
		//	}
		//	break;
		//case WM_LBUTTONDOWN:
		//	mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		//	mouseButtons.left = true;
		//	break;
		//case WM_RBUTTONDOWN:
		//	mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		//	mouseButtons.right = true;
		//	break;
		//case WM_MBUTTONDOWN:
		//	mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
		//	mouseButtons.middle = true;
		//	break;
		//case WM_LBUTTONUP:
		//	mouseButtons.left = false;
		//	break;
		//case WM_RBUTTONUP:
		//	mouseButtons.right = false;
		//	break;
		//case WM_MBUTTONUP:
		//	mouseButtons.middle = false;
		//	break;
		//case WM_MOUSEWHEEL:
		//{
		//	short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//	camera.translate(glm::vec3(0.0f, 0.0f, (float)wheelDelta * 0.005f));
		//	viewUpdated = true;
		//	break;
		//}
		//case WM_MOUSEMOVE:
		//{
		//	handleMouseMove(LOWORD(lParam), HIWORD(lParam));
		//	break;
		//}
		//case WM_SIZE:
		//	if ((prepared) && (wParam != SIZE_MINIMIZED))
		//	{
		//		if ((resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED)))
		//		{
		//			destWidth = LOWORD(lParam);
		//			destHeight = HIWORD(lParam);
		//			windowResize();
		//		}
		//	}
		//	break;
		//case WM_GETMINMAXINFO:
		//{
		//	LPMINMAXINFO minMaxInfo = (LPMINMAXINFO)lParam;
		//	minMaxInfo->ptMinTrackSize.x = 64;
		//	minMaxInfo->ptMinTrackSize.y = 64;
		//	break;
		//}
		//case WM_ENTERSIZEMOVE:
		//	resizing = true;
		//	break;
		//case WM_EXITSIZEMOVE:
		//	resizing = false;
		//	break;
	}
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void HandleTouchScreenEvent(Application* application, int32_t action)
{
	//switch (action)
	//{
	//	case AMOTION_EVENT_ACTION_UP:
	//	{
	//		vulkanExample->lastTapTime = AMotionEvent_getEventTime(event);
	//		vulkanExample->touchPos.x = AMotionEvent_getX(event, 0);
	//		vulkanExample->touchPos.y = AMotionEvent_getY(event, 0);
	//		vulkanExample->touchTimer = 0.0;
	//		vulkanExample->touchDown = false;
	//		vulkanExample->camera.keys.up = false;

	//		// Detect single tap
	//		int64_t eventTime = AMotionEvent_getEventTime(event);
	//		int64_t downTime = AMotionEvent_getDownTime(event);
	//		if (eventTime - downTime <= vks::android::TAP_TIMEOUT) {
	//			float deadZone = (160.f / vks::android::screenDensity) * vks::android::TAP_SLOP * vks::android::TAP_SLOP;
	//			float x = AMotionEvent_getX(event, 0) - vulkanExample->touchPos.x;
	//			float y = AMotionEvent_getY(event, 0) - vulkanExample->touchPos.y;
	//			if ((x * x + y * y) < deadZone) {
	//				vulkanExample->mouseButtons.left = true;
	//			}
	//		};

	//		return 1;
	//		break;
	//	}
	//	case AMOTION_EVENT_ACTION_DOWN:
	//	{
	//		// Detect double tap
	//		int64_t eventTime = AMotionEvent_getEventTime(event);
	//		if (eventTime - vulkanExample->lastTapTime <= vks::android::DOUBLE_TAP_TIMEOUT) {
	//			float deadZone = (160.f / vks::android::screenDensity) * vks::android::DOUBLE_TAP_SLOP * vks::android::DOUBLE_TAP_SLOP;
	//			float x = AMotionEvent_getX(event, 0) - vulkanExample->touchPos.x;
	//			float y = AMotionEvent_getY(event, 0) - vulkanExample->touchPos.y;
	//			if ((x * x + y * y) < deadZone) {
	//				vulkanExample->keyPressed(TOUCH_DOUBLE_TAP);
	//				vulkanExample->touchDown = false;
	//			}
	//		}
	//		else {
	//			vulkanExample->touchDown = true;
	//		}
	//		vulkanExample->touchPos.x = AMotionEvent_getX(event, 0);
	//		vulkanExample->touchPos.y = AMotionEvent_getY(event, 0);
	//		vulkanExample->mousePos.x = AMotionEvent_getX(event, 0);
	//		vulkanExample->mousePos.y = AMotionEvent_getY(event, 0);
	//		break;
	//	}
	//	case AMOTION_EVENT_ACTION_MOVE:
	//	{
	//		bool handled = false;
	//		if (vulkanExample->settings.overlay) {
	//			ImGuiIO& io = ImGui::GetIO();
	//			handled = io.WantCaptureMouse;
	//		}
	//		if (!handled) {
	//			int32_t eventX = AMotionEvent_getX(event, 0);
	//			int32_t eventY = AMotionEvent_getY(event, 0);

	//			float deltaX = (float)(vulkanExample->touchPos.y - eventY) * vulkanExample->camera.rotationSpeed * 0.5f;
	//			float deltaY = (float)(vulkanExample->touchPos.x - eventX) * vulkanExample->camera.rotationSpeed * 0.5f;

	//			vulkanExample->camera.rotate(glm::vec3(deltaX, 0.0f, 0.0f));
	//			vulkanExample->camera.rotate(glm::vec3(0.0f, -deltaY, 0.0f));

	//			vulkanExample->viewChanged();

	//			vulkanExample->touchPos.x = eventX;
	//			vulkanExample->touchPos.y = eventY;
	//		}
	//		break;
	//	}
	//	default:
	//	{
	//		return 0;
	//		break;
	//	}
	//}
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
				HandleTouchScreenEvent(application, action);
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