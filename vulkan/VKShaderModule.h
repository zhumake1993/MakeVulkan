#pragma once

#include "Common.h"
#include "NonCopyable.h"

struct VKDevice;

struct VKShaderModule : public NonCopyable
{
	VKShaderModule(VKDevice* vkDevice, std::string const &filename);
	~VKShaderModule();

	VkShaderModule shaderModule = VK_NULL_HANDLE;

private:

	VkDevice device = VK_NULL_HANDLE;
};