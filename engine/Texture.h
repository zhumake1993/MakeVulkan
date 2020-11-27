#pragma once

#include "Common.h"

struct VKImage;

class Texture
{

public:

	Texture();
	~Texture();

	void LoadFromFile(const std::string& filename);
	VkImageView GetView();

private:

	// 使用stb_image库读取文件，不支持mipmap
	std::vector<char> ReadImageUsingSTB(const std::string& filename);

	// 使用ktxvulkan库读取ktx文件，支持mipmap
	std::vector<char> ReadImageUsingKTX(const std::string& filename);

private:

	//uint32_t m_DataSize;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_MipLevels;

	VKImage* m_Image = nullptr;
};