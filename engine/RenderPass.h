#pragma once

#include "GfxDeviceObjects.h"

class Image;
class Attachment;

class RenderPass
{
	struct Subpass
	{
		mkVector<int> inputs;
		mkVector<int> colors;
		int depth;
	};

public:

	RenderPass(RenderPassKey& renderPassKey);
	virtual ~RenderPass();

	RenderPassKey& GetKey();
	uint32_t GetWidth();
	uint32_t GetHeight();

	void SetAttachments(const mkVector<Attachment*> actualAttachments);

protected:

	RenderPassKey m_RenderPassKey;

	mkVector<Image*> m_Images;
};