#include "Tools.h"
#include <fstream>

std::string VulkanErrorToString(VkResult errorCode) {
	switch (errorCode)
	{
#define STR(r) case VK_##r: return #r
		STR(NOT_READY);
		STR(TIMEOUT);
		STR(EVENT_SET);
		STR(EVENT_RESET);
		STR(INCOMPLETE);
		STR(ERROR_OUT_OF_HOST_MEMORY);
		STR(ERROR_OUT_OF_DEVICE_MEMORY);
		STR(ERROR_INITIALIZATION_FAILED);
		STR(ERROR_DEVICE_LOST);
		STR(ERROR_MEMORY_MAP_FAILED);
		STR(ERROR_LAYER_NOT_PRESENT);
		STR(ERROR_EXTENSION_NOT_PRESENT);
		STR(ERROR_FEATURE_NOT_PRESENT);
		STR(ERROR_INCOMPATIBLE_DRIVER);
		STR(ERROR_TOO_MANY_OBJECTS);
		STR(ERROR_FORMAT_NOT_SUPPORTED);
		STR(ERROR_SURFACE_LOST_KHR);
		STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		STR(SUBOPTIMAL_KHR);
		STR(ERROR_OUT_OF_DATE_KHR);
		STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		STR(ERROR_VALIDATION_FAILED_EXT);
		STR(ERROR_INVALID_SHADER_NV);
#undef STR
	default:
		return "UNKNOWN_ERROR";
	}
}

std::string PhysicalDeviceTypeString(VkPhysicalDeviceType type)
{
	switch (type)
	{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_##r: return #r
		STR(OTHER);
		STR(INTEGRATED_GPU);
		STR(DISCRETE_GPU);
		STR(VIRTUAL_GPU);
#undef STR
	default: return "UNKNOWN_DEVICE_TYPE";
	}
}

bool CheckExtensionAvailability(const char *extensionName, const std::vector<VkExtensionProperties> &availableExtensions) {
	for (size_t i = 0; i < availableExtensions.size(); ++i) {
		if (strcmp(availableExtensions[i].extensionName, extensionName) == 0) {
			return true;
		}
	}
	return false;
}

bool CheckLayerAvailability(const char *layerName, const std::vector<VkLayerProperties> &availableLayers) {
	for (size_t i = 0; i < availableLayers.size(); ++i) {
		if (strcmp(availableLayers[i].layerName, layerName) == 0) {
			return true;
		}
	}
	return false;
}

std::vector<char> GetBinaryFileContents(std::string const &filename) {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	// Load shader from compressed asset
	AAsset* asset = AAssetManager_open(androidApp->activity->assetManager, filename.c_str(), AASSET_MODE_STREAMING);
	assert(asset);
	size_t shaderSize = AAsset_getLength(asset);
	assert(shaderSize > 0);

	std::vector<char> shaderCode(shaderSize);
	AAsset_read(asset, shaderCode.data(), shaderSize);
	AAsset_close(asset);
#else
	std::ifstream file(filename, std::ios::binary);
	if (file.fail()) {
		LOG("Could not open file: %s\n", filename.c_str());
		return std::vector<char>();
	}

	std::streampos begin, end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();

	std::vector<char> shaderCode(static_cast<size_t>(end - begin));
	file.seekg(0, std::ios::beg);
	file.read(&shaderCode[0], end - begin);
	file.close();
#endif

	return shaderCode;
}

const std::string GetAssetPath()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	return "";
#else
	return "../data/";
#endif
}