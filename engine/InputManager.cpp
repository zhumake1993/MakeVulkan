#include "InputManager.h"

Input input;

void Input::Tick()
{
#if defined(_WIN32)
	input.oldPos = input.pos;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	input.oldPos0 = input.pos0;
	input.oldPos1 = input.pos1;
#endif
}
