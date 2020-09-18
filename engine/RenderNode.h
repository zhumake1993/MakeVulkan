#pragma once

#include "Common.h"

class Mesh;
class VulkanBuffer;

struct RenderNode {

	glm::mat4 m_World;
	uint32_t m_NumFramesDirty = global::frameResourcesCount;
	uint32_t m_ObjectUBIndex = -1;
	Mesh* m_Mesh;

public:

	VulkanBuffer* GetVertexBuffer();
	VulkanBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();
};

//class RenderNode
//{
//
//public:
//
//	RenderNode();
//	~RenderNode();
//
//	//
//
//private:
//
//	//
//
//public:
//
//	//
//
//private:
//
//	Mesh* m_Mesh;
//};