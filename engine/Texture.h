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

	// 使用stb_image库读取文件，不支持mipmap（懒得研究）
	void ReadImageUsingSTB(const std::string& filename);

	// 使用ktxvulkan库读取ktx文件，支持mipmap
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