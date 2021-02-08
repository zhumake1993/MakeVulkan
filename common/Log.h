#pragma once

#include "Env.h"

#if defined(_WIN32)
#define LOG(...) printf(__VA_ARGS__)
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "MakeVulkan", __VA_ARGS__))
#endif

#define LOGE(...)				\
{								\
	LOG(__VA_ARGS__);			\
	assert(false);				\
}