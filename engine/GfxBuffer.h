#pragma once

#include "NonCopyable.h"
#include "GfxTypes.h"

class GfxBuffer : NonCopyable
{
public:

	GfxBuffer(GfxBufferUsage bufferUsage, GfxBufferMode bufferMode, uint64_t size)
		: m_BufferUsage(bufferUsage)
		, m_BufferMode(bufferMode)
		, m_Size(size)
	{}
	virtual ~GfxBuffer() {}

protected:

	GfxBufferUsage m_BufferUsage;
	GfxBufferMode m_BufferMode;
	uint64_t m_Size;
};