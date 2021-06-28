#include "ResourceManager.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "Texture.h"

ResourceManager gResourceManager;

ResourceManager & GetResourceManager()
{
	return gResourceManager;
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}


Attachment * ResourceManager::CreateAttachment(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height)
{
	if (imageTypeMask & kImageSwapChainBit)
	{
		format = GetGfxDevice().GetSwapChainFormat();
		VkExtent2D extent = GetGfxDevice().GetSwapChainExtent();
		width = extent.width;
		height = extent.height;
	}

	return new Attachment(imageTypeMask, format, width, height);
}

Attachment * ResourceManager::CreateTempAttachment(int imageTypeMask, VkFormat format, uint32_t width, uint32_t height)
{
	Attachment* attachment = CreateAttachment(imageTypeMask, format, width, height);
	m_TempAttachments.push_back(attachment);
	return attachment;
}

void ResourceManager::ReleaseTempAttachment()
{
	for (auto p : m_TempAttachments) { RELEASE(p); }
	m_TempAttachments.clear();
}