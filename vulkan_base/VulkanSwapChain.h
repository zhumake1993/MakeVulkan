#pragma once

#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"

class VulkanSwapChain
{

public:

	VulkanSwapChain(VulkanDevice* vulkanDevice, VulkanSurface* surface);
	~VulkanSwapChain();

	void							CleanUp();
	uint32_t						AcquireNextImage(VkSemaphore imageAvailableSemaphores);
	void							QueuePresent(uint32_t imageIndex, VkSemaphore finishedRenderingSemaphores);

private:

	void							RecreateSwapChain();
	void							CheckSurfaceStats();
	uint32_t						GetSwapChainNumImages();
	VkSurfaceFormatKHR				GetSwapChainFormat();
	VkExtent2D						GetSwapChainExtent();
	VkImageUsageFlags				GetSwapChainUsageFlags();
	VkSurfaceTransformFlagBitsKHR	GetSwapChainTransform();
	VkPresentModeKHR				GetSwapChainPresentMode();

public:

	VkSurfaceCapabilitiesKHR		m_SurfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
	std::vector<VkPresentModeKHR>	m_PresentModes;

	uint32_t						m_NumberOfImages;
	VkSurfaceFormatKHR				m_Format;
	VkExtent2D						m_Extent;
	VkImageUsageFlags				m_Usage;
	VkSurfaceTransformFlagBitsKHR	m_Transform;
	VkPresentModeKHR				m_PresentMode;

	VkSwapchainKHR					m_SwapChain = VK_NULL_HANDLE;

	std::vector<VkImage>			m_SwapChainImages;
	std::vector<VkImageView>		m_SwapChainImageViews;

private:

	VulkanDevice*					m_VulkanDevice = nullptr;
	VulkanSurface*					m_VulkanSurface = nullptr;
};