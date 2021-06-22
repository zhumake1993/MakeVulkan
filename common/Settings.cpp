#include "Settings.h"

#if defined(_WIN32)
std::string consoleTitle = "Console Title";
std::string windowClassName = "Window Class Name";
std::string windowTitleName = "Window Title Name";
#endif

uint32_t windowWidth = 1280;
uint32_t windowHeight = 720;

#if defined(_WIN32)
std::string AssetPath = "D:/Vulkan/MakeVulkan/data/";
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
std::string AssetPath = "";
#endif

#ifdef _WIN32
#if USE_VISUAL_LEAK_DETECTOR
#include "vld.h"
#endif
#endif