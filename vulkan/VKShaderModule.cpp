#include "VKShaderModule.h"

#include "DeviceProperties.h"
#include "Tools.h"

#include "VKDevice.h"

// ²âÊÔglslangºÍsmolv
//#include "ShaderCompiler.hpp"
//#include "smolv/smolv.h"

bool BeginsWith(const char* str, const char* prefix)
{
	for (;;)
	{
		// end of prefix? we're done here (successful match).
		char p = *prefix++;
		if (p == 0)
			return true;

		// mismatch on current? fail.
		if (*str++ != p)
			return false;
	}
}
static bool SpirvDebugNameFilter(const char* name)
{
	if (!name)
		return false;

	// There are issues in some released Vulkan implementations on Adreno and Mali (case 963224 and sub-cases)
	// The most reliable workaround so far is to preserve OpName of vertex shader outputs.
	return BeginsWith(name, "vs_TEXCOORD");
}

VKShaderModule::VKShaderModule(VKDevice * vkDevice, std::string const & filename) :
	device(vkDevice->device)
{
	const std::vector<char> code = GetBinaryFileContents(filename);
	//const std::vector<unsigned int> code = CompileGLSL(filename);
	if (code.size() == 0) {
		assert(false);
	}

	//// smolv
	//smolv::ByteArray smolvData;
	//const uint8_t* spirvCode = reinterpret_cast<const uint8_t*>(&code[0]);
	//const uint32_t spirvCodeSize = static_cast<uint32_t>(code.size() * sizeof(uint32_t));
	//smolv::Encode(spirvCode, spirvCodeSize, smolvData, 0, SpirvDebugNameFilter);
	//spirvCode = &smolvData[0];
	//uint32_t compressedCodeSize = static_cast<uint32_t>(smolvData.size());

	//// decode
	//const uint32_t *dataPtr = reinterpret_cast<const uint32_t*>(spirvCode);
	//uint32_t size = (uint32_t)smolv::GetDecodedBufferSize(dataPtr, spirvCodeSize);
	//std::vector<uint32_t> buffer(size / 4);
	////buffer.resize_uninitialized(size / 4);
	//bool success = smolv::Decode(dataPtr, compressedCodeSize, buffer.data(), size);
	//assert(success);

	VkShaderModuleCreateInfo moduleCreateInfo = {};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.flags = 0;

	moduleCreateInfo.codeSize = code.size();
	moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	//moduleCreateInfo.codeSize = code.size() * sizeof(unsigned int);
	//moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	//moduleCreateInfo.codeSize = buffer.size() * sizeof(unsigned int);
	//moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	VK_CHECK_RESULT(vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule));
}

VKShaderModule::~VKShaderModule()
{
	if (device != VK_NULL_HANDLE && shaderModule != VK_NULL_HANDLE) {
		vkDestroyShaderModule(device, shaderModule, nullptr);
		shaderModule = VK_NULL_HANDLE;
	}
}
