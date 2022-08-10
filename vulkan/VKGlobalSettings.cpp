#include "VKGlobalSettings.h"
#include "Log.h"

namespace vk
{
	VKGlobalSettings gVKGlobalSettings;

	VKGlobalSettings::VKGlobalSettings()
	{
		// Instance

#if defined(_WIN32)
		enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation"); // 安卓上面使用这个layer有点麻烦，待研究
#endif

		enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
		enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

		// Device

		enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	VKGlobalSettings::~VKGlobalSettings()
	{
	}

	void VKGlobalSettings::Print()
	{
		LOG("[VKGlobalSettings]\n");

		LOG("enabled instance layer ( %d ):", enabledInstanceLayers.size());
		for (int i = 0; i < enabledInstanceLayers.size(); i++)
		{
			LOG(" %s", enabledInstanceLayers[i]);
		}
		LOG("\n");

		LOG("enabled instance extensions ( %d ):", enabledInstanceExtensions.size());
		for (int i = 0; i < enabledInstanceExtensions.size(); i++)
		{
			LOG(" %s", enabledInstanceExtensions[i]);
		}
		LOG("\n");

		LOG("enabled device extensions ( %d ):", enabledDeviceExtensions.size());
		for (int i = 0; i < enabledDeviceExtensions.size(); i++)
		{
			LOG(" %s", enabledDeviceExtensions[i]);
		}
		LOG("\n");

		LOG("memoryAlignment: %d\n", static_cast<int>(memoryAlignment));
		LOG("memoryBlockSize: %d\n", static_cast<int>(memoryBlockSize));

		LOG("\n");
	}

	VKGlobalSettings & GetVKGlobalSettings()
	{
		return gVKGlobalSettings;
	}
}