#include "VKSampler.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"

VKSampler::VKSampler(VKDevice* vkDevice, VkSamplerCreateInfo& ci) :
	device(vkDevice->device)
{
	VK_CHECK_RESULT(vkCreateSampler(device, &ci, nullptr, &sampler));
}

VKSampler::~VKSampler()
{
	if (device != VK_NULL_HANDLE) {
		if (sampler != VK_NULL_HANDLE) {
			vkDestroySampler(device, sampler, nullptr);
			sampler = VK_NULL_HANDLE;
		}
	}
}