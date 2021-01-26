#pragma once

#include "Env.h"
#include "NonCopyable.h"
#include "GfxTypes.h"

class Buffer : public NonCopyable
{
public:

	Buffer(BufferType bufferType);
	virtual ~Buffer();

	BufferType GetBufferType() { return m_BufferType; }

protected:

	BufferType m_BufferType;
};