#pragma once

#include "Common.h"

class Mesh;
struct VKBuffer;

class RenderNode 
{

public:

	void SetObjectUBIndex(uint32_t index);
	uint32_t GetObjectUBIndex();

	void SetWorldMatrix(glm::mat4 world);
	glm::mat4 GetWorldMatrix();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();

	VKBuffer* GetVertexBuffer();
	VKBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();

	// todo
	uint32_t m_NumFramesDirty = FrameResourcesCount;

private:

	uint32_t m_ObjectUBIndex = -1;
	glm::mat4 m_WorldMatrix;
	Mesh* m_Mesh;

	//uint32_t m_NumFramesDirty = FrameResourcesCount;
};