#pragma once

#include "Env.h"

class Image;

class Texture
{

public:

	Texture(const std::string& name);
	virtual ~Texture();

	void LoadFromFile(const std::string& filename);

	Image* GetImage();

	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeight() { return m_Height; }
	uint32_t GetMipCount() { return m_MipLevels; }

private:

	// ʹ��stb_image���ȡ�ļ�����֧��mipmap�������о���
	void ReadImageUsingSTB(const std::string& filename);

	// ʹ��ktxvulkan���ȡktx�ļ���֧��mipmap
	void ReadImageUsingKTX(const std::string& filename);

private:

	std::string m_Name;

	std::vector<char> m_ImageData;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_MipLevels = 1;
	std::vector<uint64_t> m_MipOffsets;

	Image* m_Image = nullptr;
};