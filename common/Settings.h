#pragma once

#include "Env.h"

// ʹ��Visual Leak Detector������PC��
// Ϊ�˷��㣬�Ұ�vld�İ���·��ֱ�Ӽӵ�ϵͳ��path��ȥ��
#ifdef _WIN32
#define USE_VISUAL_LEAK_DETECTOR 0
#endif

#if defined(_WIN32)
extern std::string consoleTitle;
extern std::string windowClassName;
extern std::string windowTitleName;
#endif

extern uint32_t windowWidth;
extern uint32_t windowHeight;

extern std::string AssetPath;