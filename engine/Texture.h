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

	// 使用stb_image库读取文件，不支持mipmap
	std::vector<char> ReadImageUsingSTB(const std::string& filename, uint32_t& width, uint32_t& height);

	// 使用ktxvulkan库读取ktx文件，支持mipmap
	std::vector<char> ReadImageUsingKTX(const std::string& filename, uint32_t& width, uint32_t& height);

private:

	std::string m_Name;

	Image* m_Image = nullptr;
};