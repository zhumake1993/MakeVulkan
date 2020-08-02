//#pragma once
//
////class VulkanXXXXXXX
////{
////
////public:
////
////	VulkanXXXXXXX();
////	~VulkanXXXXXXX();
////
////	//
////
////private:
////
////	//
////
////public:
////
////	//
////};
//
//#pragma once
//
//#include "VKDevice.h"
//
//namespace Vulkan {
//
//	class Fence {
//	public:
//		Fence(VKDevice* vkDevice, bool signaled);
//		~Fence();
//
//		VkFence & operator*() {
//			return m_Fence;
//		}
//
//		VkFence const & operator*() const {
//			return m_Fence;
//		}
//
//		void Reset();
//
//		void Wait(uint64_t timeout);
//
//	private:
//		VKDevice* m_VKDevice = nullptr;
//		VkFence m_Fence = VK_NULL_HANDLE;
//	};
//
//	bool ResetFences(VKDevice * vkDevice, std::vector<Fence> const & fences);
//
//	bool WaitForFences(VKDevice * vkDevice, std::vector<Fence> const & fences, VkBool32 wait_for_all, uint64_t timeout);
//
//	class Semaphore {
//	public:
//		Semaphore(VKDevice* vkDevice);
//		~Semaphore();
//
//		VkSemaphore & operator*() {
//			return m_VkSemaphore;
//		}
//
//		VkSemaphore const & operator*() const {
//			return m_VkSemaphore;
//		}
//	private:
//		VKDevice* m_VKDevice = nullptr;
//		VkSemaphore m_VkSemaphore = VK_NULL_HANDLE;
//	};
//}
//
