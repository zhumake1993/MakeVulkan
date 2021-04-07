#pragma once

#include "Env.h"
#include "GfxDeviceObjects.h"

class Image;
class Attachment;

class RenderPass
{
	struct Subpass
	{
		std::vector<int> inputs;
		std::vector<int> colors;
		int depth;
	};

public:

	RenderPass(RenderPassKey& renderPassKey);
	virtual ~RenderPass();

	RenderPassKey& GetKey();
	uint32_t GetWidth();
	uint32_t GetHeight();

	void SetAttachments(const std::vector<Attachment*> actualAttachments);

protected:

	RenderPassKey m_RenderPassKey;

	std::vector<Image*> m_Images;
};