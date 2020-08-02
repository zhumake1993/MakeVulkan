//#include "Synchronization.h"
//
//namespace Vulkan {
//
//	Fence::Fence(VKDevice * vkDevice, bool signaled)
//	{
//		m_VKDevice = vkDevice;
//
//		VkFenceCreateInfo fence_create_info = {
//		  VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,          // VkStructureType        sType
//		  nullptr,                                      // const void           * pNext
//		  signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u, // VkFenceCreateFlags     flags
//		};
//
//		VkResult result = vkCreateFence(m_VKDevice->m_Device, &fence_create_info, nullptr, &m_Fence);
//		if (VK_SUCCESS != result) {
//			std::cout << "Could not create a fence." << std::endl;
//		}
//	}
//
//	Fence::~Fence()
//	{
//		if (VK_NULL_HANDLE != m_Fence) {
//			vkDestroyFence(m_VKDevice->m_Device, m_Fence, nullptr);
//			m_Fence = VK_NULL_HANDLE;
//		}
//	}
//
//	void Fence::Reset()
//	{
//		VkResult result = vkResetFences(m_VKDevice->m_Device, 1, &m_Fence);
//		if (VK_SUCCESS != result) {
//			std::cout << "Error occurred when tried to reset fences." << std::endl;
//		}
//	}
//
//	void Fence::Wait(uint64_t timeout)
//	{
//		VkResult result = vkWaitForFences(m_VKDevice->m_Device, 1, &m_Fence, 0, timeout);
//		if (VK_SUCCESS != result) {
//			std::cout << "Waiting on fence failed." << std::endl;
//		}
//	}
//
//	bool ResetFences(VKDevice * vkDevice, std::vector<Fence> const & fences)
//	{
//		std::vector<VkFence> vkfences;
//		for (auto & fence : fences) {
//			vkfences.push_back(*fence);
//		}
//
//		if (fences.size() > 0) {
//			VkResult result = vkResetFences(vkDevice->m_Device, static_cast<uint32_t>(vkfences.size()), vkfences.data());
//			if (VK_SUCCESS != result) {
//				std::cout << "Error occurred when tried to reset fences." << std::endl;
//				return false;
//			}
//			return VK_SUCCESS == result;
//		}
//		return false;
//	}
//
//	bool WaitForFences(VKDevice * vkDevice, std::vector<Fence> const & fences, VkBool32 wait_for_all, uint64_t timeout)
//	{
//		std::vector<VkFence> vkfences;
//		for (auto & fence : fences) {
//			vkfences.push_back(*fence);
//		}
//
//		if (fences.size() > 0) {
//			VkResult result = vkWaitForFences(vkDevice->m_Device, static_cast<uint32_t>(vkfences.size()), vkfences.data(), wait_for_all, timeout);
//			if (VK_SUCCESS != result) {
//				std::cout << "Waiting on fence failed." << std::endl;
//				return false;
//			}
//			return true;
//		}
//		return false;
//	}
//
//	Semaphore::Semaphore(VKDevice * vkDevice)
//	{
//		m_VKDevice = vkDevice;
//
//		VkSemaphoreCreateInfo semaphore_create_info = {
//		  VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,    // VkStructureType            sType
//		  nullptr,                                    // const void               * pNext
//		  0                                           // VkSemaphoreCreateFlags     flags
//		};
//
//		VkResult result = vkCreateSemaphore(m_VKDevice->m_Device, &semaphore_create_info, nullptr, &m_VkSemaphore);
//		if (VK_SUCCESS != result) {
//			std::cout << "Could not create a semaphore." << std::endl;
//		}
//	}
//
//	Semaphore::~Semaphore()
//	{
//		if (VK_NULL_HANDLE != m_VkSemaphore) {
//			vkDestroySemaphore(m_VKDevice->m_Device, m_VkSemaphore, nullptr);
//			m_VkSemaphore = VK_NULL_HANDLE;
//		}
//	}
//}