#pragma once

#include "Common.h"

struct VKImage;

class Texture
{

public:

	Texture();
	~Texture();

	void LoadFromFile(std::string filename);
	VkImageView GetView();

private:

	//

private:

	VKImage* m_Image = nullptr;
};