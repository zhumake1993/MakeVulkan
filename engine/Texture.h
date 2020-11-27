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

	// ʹ��stb_image���ȡ�ļ�����֧��mipmap
	std::vector<char> ReadImageUsingSTB(const std::string& filename);

	// ʹ��ktxvulkan���ȡktx�ļ���֧��mipmap
	std::vector<char> ReadImageUsingKTX(const std::string& filename);

private:

	//uint32_t m_DataSize;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_MipLevels;

	VKImage* m_Image = nullptr;
};