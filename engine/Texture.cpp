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
	GetGfxDevice().ReleaseImage(m_Image);
	RELEASE(m_Image);
}

void Texture::LoadFromFile(const std::string& filename, VkFormat format, bool isCubemap)
{
	m_ImageData.clear();

	m_Format = format;
	m_FaceCount = isCubemap ? 6 : 1;

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

	m_Image = device.CreateImage(m_Format, m_Width, m_Height, m_MipLevels, m_LayerCount, m_FaceCount);
	device.UpdateImage(m_Image, m_ImageData.data(), m_ImageData.size(), m_Offsets);
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
	m_Offsets = { { {0} } };

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
	m_LayerCount = ktxTexture->numLayers;

	ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
	uint32_t dataSize = static_cast<uint32_t>(ktxTexture_GetSize(ktxTexture));

	m_ImageData.resize(dataSize);
	memcpy(m_ImageData.data(), ktxTextureData, dataSize);

	/*
		Cube map arrays in ktx are stored with a layout like this:
		- Mip Level 0
			- Layer 0 (= Cube map 0)
				- Face +X
				- Face -X
				- Face +Y
				- Face -Y
				- Face +Z
				- Face -Z
			- Layer 1 (= Cube map 1)
				- Face +X
				...
		- Mip Level 1
			- Layer 0 (= Cube map 0)
				- Face +X
				...
			- Layer 1 (= Cube map 1)
				- Face +X
				...
	*/
	
	// 计算每个face, 每个layer，每个mip的offset，vkCmdCopyBufferToImage需要
	m_Offsets.resize(m_FaceCount);
	for (uint32_t face = 0; face < m_FaceCount; face++)
	{
		m_Offsets[face].resize(m_LayerCount);
		for (uint32_t layer = 0; layer < m_LayerCount; layer++)
		{
			m_Offsets[face][layer].resize(m_MipLevels);
			for (uint32_t level = 0; level < m_MipLevels; level++)
			{
				ktx_size_t offset;
				KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, level, layer, face, &offset);
				assert(result == KTX_SUCCESS);

				m_Offsets[face][layer][level] = static_cast<uint64_t>(offset);
			}
		}
	}

	ktxTexture_Destroy(ktxTexture);
}
