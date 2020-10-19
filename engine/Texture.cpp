#include "Texture.h"
#include "Tools.h"
#include "VulkanDriver.h"
#include "VKImage.h"

Texture::Texture()
{
}

Texture::~Texture()
{
	RELEASE(m_Image);
}

void Texture::LoadFromFile(std::string filename)
{
	uint32_t width = 0, height = 0, dataSize = 0;
	std::vector<char> imageData = GetImageData(filename, 4, &width, &height, nullptr, &dataSize);
	if (imageData.size() == 0) {
		assert(false);
	}

	VKImageCI(imageCI);
	imageCI.extent.width = width;
	imageCI.extent.height = height;
	VKImageViewCI(imageViewCI);

	auto& driver = GetVulkanDriver();
	m_Image = driver.CreateVKImage(imageCI, imageViewCI);
	driver.UploadVKImage(m_Image, imageData.data(), dataSize);
}

VkImageView Texture::GetView()
{
	return m_Image->view;
}
