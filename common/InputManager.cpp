#include "InputManager.h"

InputManager inputManager;

void InputManager::Tick()
{
#if defined(_WIN32)
	oldPos = pos;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	oldPos0 = pos0;
	oldPos1 = pos1;
#endif
}
