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

private:

	// ʹ��stb_image���ȡ�ļ�����֧��mipmap
	std::vector<char> ReadImageUsingSTB(const std::string& filename, uint32_t& width, uint32_t& height);

	// ʹ��ktxvulkan���ȡktx�ļ���֧��mipmap
	std::vector<char> ReadImageUsingKTX(const std::string& filename, uint32_t& width, uint32_t& height);

private:

	std::string m_Name;

	Image* m_Image = nullptr;
};