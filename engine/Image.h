#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "GfxTypes.h"

class Image : public NonCopyable
{
public:

	Image(ImageType imageType, VkFormat format, uint32_t width, uint32_t height);
	virtual ~Image();

	ImageType GetImageType() { return m_ImageType; }
	VkFormat GetFormat() { return m_Format; }
	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeight() { return m_Height; }

protected:

	ImageType m_ImageType;
	VkFormat m_Format;
	uint32_t m_Width;
	uint32_t m_Height;
};