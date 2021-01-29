#include "Settings.h"
#include "Env.h"

#if defined(_WIN32)
std::string consoleTitle = "Console Title";
std::string windowClassName = "Window Class Name";
std::string windowTitleName = "Window Title Name";
#endif

uint32_t windowWidth = 1280;
uint32_t windowHeight = 720;

#if defined(_WIN32)
std::string AssetPath = "C:/Users/1/Desktop/MakeVulkan/data/";
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
std::string AssetPath = "";
#endif