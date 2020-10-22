#pragma once

#include "Common.h"
#include "Transform.h"

class Mesh;
struct VKBuffer;

class RenderNode 
{

public:

	RenderNode(uint32_t objectUBIndex);
	~RenderNode();

	void SetObjectUBIndex(uint32_t index);
	uint32_t GetObjectUBIndex();

	Transform& GetTransform();

	void SetWorldMatrix(glm::mat4& world);
	glm::mat4 GetWorldMatrix();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();

	VKBuffer* GetVertexBuffer();
	VKBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();

	void SetDirty();
	bool IsDirty();
	void Clean();

private:

	uint32_t m_ObjectUBIndex;
	Transform m_Transform;
	Mesh* m_Mesh;

	uint32_t m_NumFramesDirty = FrameResourcesCount;
};