#include "VKSampler.h"
#include "Tools.h"

VKSampler::VKSampler(VkDevice device, VkSamplerCreateInfo & ci):
	m_Device(device)
{
	VK_CHECK_RESULT(vkCreateSampler(m_Device, &ci, nullptr, &m_Sampler));
}

VKSampler::~VKSampler()
{
	if (m_Device != VK_NULL_HANDLE) {
		if (m_Sampler != VK_NULL_HANDLE) {
			vkDestroySampler(m_Device, m_Sampler, nullptr);
			m_Sampler = VK_NULL_HANDLE;
		}
	}
}

VkSampler VKSampler::GetSampler()
{
	return m_Sampler;
}
