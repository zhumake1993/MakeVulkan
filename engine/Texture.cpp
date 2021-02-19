#include "Texture.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <ktxvulkan.h>

Texture::Texture(const std::string& name) :
	m_Name(name)
{
}

Texture::~Texture()
{
	RELEASE(m_Image);
}

void Texture::LoadFromFile(const std::string& filename)
{
	m_ImageData.clear();

	size_t last = filename.find_last_of('.');
	std::string suffix = filename.substr(last + 1);
	if (suffix == "ktx")
	{
		ReadImageUsingKTX(filename);
	}
	else
	{
		ReadImageUsingSTB(filename);
	}

	auto& device = GetGfxDevice();

	m_Image = device.CreateImage(kImageType2D, VK_FORMAT_R8G8B8A8_UNORM, m_Width, m_Height, m_MipLevels);
	device.UpdateImage(m_Image, m_ImageData.data(), m_ImageData.size(), m_MipOffsets);
}

Image * Texture::GetImage()
{
	return m_Image;
}

void Texture::ReadImageUsingSTB(const std::string& filename)
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
		return;
	}

	uint32_t dataSize = (tmpWidth) * (tmpHeight) * (requestedComponents <= 0 ? tmpComponents : requestedComponents);
	m_Width = tmpWidth;
	m_Height = tmpHeight;
	m_MipLevels = 1;
	m_MipOffsets = { 0 };

	m_ImageData.resize(dataSize);
	memcpy(m_ImageData.data(), imageData, dataSize);

	stbi_image_free(imageData);
}

void Texture::ReadImageUsingKTX(const std::string& filename)
{
	std::vector<char> fileData = GetBinaryFileContents(filename);

	ktxTexture* ktxTexture;
	ktxResult result = KTX_SUCCESS;
	result = ktxTexture_CreateFromMemory(reinterpret_cast<ktx_uint8_t*>(&fileData[0]), static_cast<ktx_size_t>(fileData.size()), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
	assert(result == KTX_SUCCESS);

	m_Width = ktxTexture->baseWidth;
	m_Height = ktxTexture->baseHeight;
	m_MipLevels = ktxTexture->numLevels;

	ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
	uint32_t dataSize = static_cast<uint32_t>(ktxTexture_GetSize(ktxTexture));

	m_ImageData.resize(dataSize);
	memcpy(m_ImageData.data(), ktxTextureData, dataSize);

	// 计算每个mip的offset，vkCmdCopyBufferToImage需要逐个mip进行copy
	m_MipOffsets.resize(m_MipLevels);
	for (uint32_t i = 0; i < m_MipLevels; i++)
	{
		ktx_size_t offset;
		KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
		assert(ret == KTX_SUCCESS);
		m_MipOffsets[i] = static_cast<uint64_t>(offset);
	}

	ktxTexture_Destroy(ktxTexture);
}
