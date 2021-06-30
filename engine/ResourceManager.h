#pragma once

#include "NonCopyable.h"
#include "mkVector.h"
#include "VKIncludes.h"//todo

class Attachment;

class ResourceManager : public NonCopyable
{
public:

	ResourceManager();
	virtual ~ResourceManager();

	Attachment* CreateAttachment(int imageTypeMask, VkFormat format = VK_FORMAT_UNDEFINED, uint32_t width = 0, uint32_t height = 0); // kImageSwapChain类型不需要其他参数
	Attachment* CreateTempAttachment(int imageTypeMask, VkFormat format = VK_FORMAT_UNDEFINED, uint32_t width = 0, uint32_t height = 0);

	void ReleaseTempAttachment();

private:

	mkVector<Attachment*> m_TempAttachments;
};

ResourceManager& GetResourceManager();