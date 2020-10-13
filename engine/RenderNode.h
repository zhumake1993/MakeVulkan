#pragma once

#include "Common.h"

class Mesh;
struct VKBuffer;

class RenderNode 
{

public:

	glm::mat4 m_World;
	uint32_t m_NumFramesDirty = FrameResourcesCount;
	uint32_t m_ObjectUBIndex = -1;
	Mesh* m_Mesh;

public:

	VKBuffer* GetVertexBuffer();
	VKBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();
};