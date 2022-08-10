#pragma once

#include "mkString.h"
#include "mkVector.h"
#include "NonCopyable.h"

// 使用Visual Leak Detector（仅限PC）
// 为了方便，我把vld的包含路径直接加到系统的path里去了
#ifdef _WIN32
#define USE_VISUAL_LEAK_DETECTOR 0
#endif

#define FrameResourcesCount 3

struct GlobalSettings : public NonCopyable
{
	GlobalSettings();
	~GlobalSettings();

	void Print();

#if defined(_WIN32)
	mkString consoleTitle{ "Console Title" };
	mkString windowClassName{ "Window Class Name" };
	mkString windowTitleName{ "Window Title Name" };
	uint32_t windowWidth = 1280;
	uint32_t windowHeight = 720;
#endif

#if defined(_WIN32)
	mkString assetPath{ "D:/Vulkan/MakeVulkan/data/" };
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	mkString assetPath{ "" };
#endif
};

GlobalSettings& GetGlobalSettings();