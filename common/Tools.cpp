#include "Tools.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "obj/tiny_obj_loader.h"

// test
#include <ktxvulkan.h>

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

std::vector<char> GetImageData(std::string const &filename, int requestedComponents, uint32_t *width, uint32_t *height, uint32_t *components, uint32_t *dataSize) {
	std::vector<char> fileData = GetBinaryFileContents(filename);
	if (fileData.size() == 0) {
		return std::vector<char>();
	}

	size_t last = filename.find_last_of('.');
	if (filename.substr(last + 1) == "ktx") {

		// ktx

		ktxTexture* ktxTexture;
		ktxResult result = KTX_SUCCESS;
		result = ktxTexture_CreateFromMemory(reinterpret_cast<ktx_uint8_t*>(&fileData[0]), static_cast<ktx_size_t>(fileData.size()), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
		assert(result == KTX_SUCCESS);

		*width = ktxTexture->baseWidth;
		*height = ktxTexture->baseHeight;
		//mipLevels = ktxTexture->numLevels;

		ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
		*dataSize = static_cast<uint32_t>(ktxTexture_GetSize(ktxTexture));

		std::vector<char> output(*dataSize);
		memcpy(&output[0], ktxTextureData, *dataSize);

		ktxTexture_Destroy(ktxTexture);

		return output;
	}
	else {

		// stb

		int tmpWidth = 0, tmpHeight = 0, tmpComponents = 0;
		unsigned char *imageData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&fileData[0]), static_cast<int>(fileData.size()), &tmpWidth, &tmpHeight, &tmpComponents, requestedComponents);
		if ((imageData == nullptr) ||
			(tmpWidth <= 0) ||
			(tmpHeight <= 0) ||
			(tmpComponents <= 0)) {
			LOG("Could not read image data!");
			return std::vector<char>();
		}

		int size = (tmpWidth) * (tmpHeight) * (requestedComponents <= 0 ? tmpComponents : requestedComponents);
		if (dataSize) {
			*dataSize = size;
		}
		if (width) {
			*width = tmpWidth;
		}
		if (height) {
			*height = tmpHeight;
		}
		if (components) {
			*components = tmpComponents;
		}

		std::vector<char> output(size);
		memcpy(&output[0], imageData, size);

		stbi_image_free(imageData);
		return output;
	}
}