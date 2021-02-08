#include "Texture.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <ktxvulkan.h>

Texture::Texture(std::string name) :
	m_Name(name)
{
}

Texture::~Texture()
{
	auto& device = GetGfxDevice();

	device.ReleaseImage(m_Image);

	RELEASE(m_Image);
}

void Texture::LoadFromFile(const std::string& filename)
{
	size_t last = filename.find_last_of('.');
	std::string suffix = filename.substr(last + 1);

	std::vector<char> imageData;
	uint32_t width;
	uint32_t height;
	if (suffix == "ktx")
	{
		imageData = ReadImageUsingKTX(filename, width, height);
	}
	else
	{
		imageData = ReadImageUsingSTB(filename, width, height);
	}

	auto& device = GetGfxDevice();

	m_Image = device.CreateImage(kImageType2D, VK_FORMAT_R8G8B8A8_UNORM, width, height);
	device.UpdateImage(m_Image, imageData.data(), imageData.size());
}

Image * Texture::GetImage()
{
	return m_Image;
}

std::vector<char> Texture::ReadImageUsingSTB(const std::string& filename, uint32_t& width, uint32_t& height)
{
	std::vector<char> fileData = GetBinaryFileContents(filename);

	int tmpWidth = 0, tmpHeight = 0, tmpComponents = 0, requestedComponents = 4;
	unsigned char *imageData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&fileData[0]), static_cast<int>(fileData.size()), &tmpWidth, &tmpHeight, &tmpComponents, requestedComponents);
	if ((imageData == nullptr) ||
		(tmpWidth <= 0) ||
		(tmpHeight <= 0) ||
		(tmpComponents <= 0))
	{
		LOG("Could not read image data from file(%s)\n", filename.c_str());
		return std::vector<char>();
	}

	uint32_t dataSize = (tmpWidth) * (tmpHeight) * (requestedComponents <= 0 ? tmpComponents : requestedComponents);
	width = tmpWidth;
	height = tmpHeight;
	//m_MipLevels = 1;todo

	std::vector<char> output(dataSize);
	memcpy(&output[0], imageData, dataSize);

	stbi_image_free(imageData);

	return output;
}

std::vector<char> Texture::ReadImageUsingKTX(const std::string& filename, uint32_t& width, uint32_t& height)
{
	std::vector<char> fileData = GetBinaryFileContents(filename);

	ktxTexture* ktxTexture;
	ktxResult result = KTX_SUCCESS;
	result = ktxTexture_CreateFromMemory(reinterpret_cast<ktx_uint8_t*>(&fileData[0]), static_cast<ktx_size_t>(fileData.size()), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
	assert(result == KTX_SUCCESS);

	width = ktxTexture->baseWidth;
	height = ktxTexture->baseHeight;
	//m_MipLevels = ktxTexture->numLevels;todo

	ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
	uint32_t dataSize = static_cast<uint32_t>(ktxTexture_GetSize(ktxTexture));

	std::vector<char> output(dataSize);
	memcpy(&output[0], ktxTextureData, dataSize);

	ktxTexture_Destroy(ktxTexture);

	return output;
	
}
