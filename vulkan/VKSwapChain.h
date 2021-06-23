#pragma once

#include <vector>
#include "VKIncludes.h"
#include "NonCopyable.h"

namespace vk
{
	class VKSwapChain : public NonCopyable
	{
	public:

		VKSwapChain(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
		~VKSwapChain();

		void CheckQueueSurfaceSupport(VkPhysicalDevice physicalDevice, int index);

		// todo
		VkSwapchainKHR& GetSwapChain() { return m_SwapChain; }
		const VkSwapchainKHR& GetSwapChain() const { return m_SwapChain; }

		void Print();

	private:

		VkSurfaceKHR							m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR							m_SwapChain = VK_NULL_HANDLE;

		uint32_t								m_ImageNum;
		VkSurfaceFormatKHR						m_Format;
		VkExtent2D								m_Extent;
		VkImageUsageFlags						m_Usage;
		VkSurfaceTransformFlagBitsKHR			m_Transform;
		VkPresentModeKHR						m_PresentMode;

		std::vector<VkImage>					m_SwapChainImages;
		std::vector<VkImageView>				m_SwapChainImageViews;

		VkInstance								m_Instance = VK_NULL_HANDLE;
		VkDevice								m_Device = VK_NULL_HANDLE;
	};
}