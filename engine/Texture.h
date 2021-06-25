#pragma once

#include "NamedObject.h"
#include <vector>

class Image;

class TextureBase : public NamedObject
{
public:

	TextureBase(const std::string& name);
	TextureBase(const std::string& name, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t faceCount);
	virtual ~TextureBase();

	VkFormat GetFormat() { return m_Format; }
	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeight() { return m_Height; }
	uint32_t GetMipCount() { return m_MipLevels; }
	uint32_t GetLayerCount() { return m_LayerCount; }
	uint32_t GetFaceCount() { return m_FaceCount; }

	Image* GetImage() { return m_Image; }

protected:

	VkFormat m_Format;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_MipLevels;
	uint32_t m_LayerCount;
	uint32_t m_FaceCount;

	Image* m_Image = nullptr;
};

class Texture : public TextureBase
{

public:

	Texture(const std::string& name);
	virtual ~Texture();

	void LoadFromFile(const std::string& filename, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, bool isCubemap = false);

private:

	// ʹ��stb_image���ȡ�ļ�����֧��mipmap�������о���
	void ReadImageUsingSTB(const std::string& filename);

	// ʹ��ktxvulkan���ȡktx�ļ���֧�ִ󲿷�����
	void ReadImageUsingKTX(const std::string& filename);

private:

	std::vector<char> m_ImageData;
	std::vector<std::vector<std::vector<uint64_t>>> m_Offsets;
};

class Attachment : public TextureBase
{
public:

	Attachment(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height);
	virtual ~Attachment();
};