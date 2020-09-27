#pragma once

#include "Common.h"

#if defined(_WIN32)

#define KEY_W 0x57
#define KEY_A 0x41
#define KEY_S 0x53
#define KEY_D 0x44
#define KEY_Q 0x51
#define KEY_E 0x45

struct Input
{
	bool key_W = false;
	bool key_S = false;
	bool key_A = false;
	bool key_D = false;
	bool key_Q = false;
	bool key_E = false;
	bool key_MouseLeft = false;
	bool key_MouseRight = false;
	glm::vec2 pos = glm::vec2(0.0f, 0.0f);
	glm::vec2 oldPos = glm::vec2(0.0f, 0.0f);

	void Tick();
};

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

struct Input
{
	// 触点数量
	int count = 0;

	// 最多支持两点触屏
	glm::vec2 pos0 = glm::vec2(0.0f, 0.0f);
	glm::vec2 oldPos0 = glm::vec2(0.0f, 0.0f);
	glm::vec2 pos1 = glm::vec2(0.0f, 0.0f);
	glm::vec2 oldPos1 = glm::vec2(0.0f, 0.0f);
};

#endif

extern Input input;