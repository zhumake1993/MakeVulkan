#include "VKShaderModule.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"

VKShaderModule::VKShaderModule(VKDevice * vkDevice, std::string const & filename) :
	device(vkDevice->device)
{
	const std::vector<char> code = GetBinaryFileContents(filename);
	if (code.size() == 0) {
		assert(false);
	}

	VkShaderModuleCreateInfo moduleCreateInfo = {};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.flags = 0;
	moduleCreateInfo.codeSize = code.size();
	moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VK_CHECK_RESULT(vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule));
}

VKShaderModule::~VKShaderModule()
{
	if (device != VK_NULL_HANDLE && shaderModule != VK_NULL_HANDLE) {
		vkDestroyShaderModule(device, shaderModule, nullptr);
		shaderModule = VK_NULL_HANDLE;
	}
}
