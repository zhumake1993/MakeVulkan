#pragma once

#include "VKIncludes.h"
#include "NonCopyable.h"

namespace vk
{
	class VKContex : public NonCopyable
	{
	public:

		VKContex();
		~VKContex();

		void Print();

	public:

		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue queue = VK_NULL_HANDLE;

		int selectedPhysicalDeviceIndex;
		int selectedQueueFamilyIndex;
	};
}