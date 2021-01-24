#pragma once

#include "Env.h"

// 使用Visual Leak Detector（仅限PC）
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