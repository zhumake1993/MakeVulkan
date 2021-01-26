#include "Image.h"

Image::Image(ImageType imageType, VkFormat format, uint32_t width, uint32_t height) :
	m_ImageType(imageType),
	m_Format(format),
	m_Width(width),
	m_Height(height)
{
}

Image::~Image()
{
}
