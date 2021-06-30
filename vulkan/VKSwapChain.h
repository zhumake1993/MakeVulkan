#pragma once

#include "mkVector.h"
#include "VKIncludes.h"
#include "NonCopyable.h"

namespace vk
{
	class VKSwapChain : public NonCopyable
	{
	public:

		VKSwapChain(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, int selectedQueueFamilyIndex);
		~VKSwapChain();

		// todo
		VkSwapchainKHR& GetSwapChain() { return m_SwapChain; }
		const VkSwapchainKHR& GetSwapChain() const { return m_SwapChain; }

		VkSurfaceFormatKHR GetSurfaceFormat() { return m_SurfaceFormat; }
		VkExtent2D GetExtent2D() { return m_Extent; }

		VkImageView GetImageView(int index) { return m_SwapChainImageViews[index]; }

		void Print();

	private:

		VkSurfaceKHR							m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR							m_SwapChain = VK_NULL_HANDLE;

		uint32_t								m_ImageNum;
		VkSurfaceFormatKHR						m_SurfaceFormat;
		VkExtent2D								m_Extent;
		VkImageUsageFlags						m_Usage;
		VkSurfaceTransformFlagBitsKHR			m_Transform;
		VkPresentModeKHR						m_PresentMode;

		mkVector<VkImage>					m_SwapChainImages;
		mkVector<VkImageView>				m_SwapChainImageViews;

		VkInstance								m_Instance = VK_NULL_HANDLE;
		VkDevice								m_Device = VK_NULL_HANDLE;
	};
}