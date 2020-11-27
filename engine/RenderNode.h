#pragma once

#include "Common.h"
#include "Transform.h"

class Mesh;
struct VKBuffer;
class Material;

class RenderNode 
{

public:

	RenderNode(uint32_t index);
	~RenderNode();

	void SetDUBIndex(uint32_t index);
	uint32_t GetDUBIndex();

	Transform& GetTransform();

	void SetWorldMatrix(glm::mat4& world);
	glm::mat4& GetWorldMatrix();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();

	void SetMaterial(Material* material);
	Material* GetMaterial();

	VKBuffer* GetVertexBuffer();
	VKBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();

	void SetDirty();
	bool IsDirty();
	void Clean();

private:

	uint32_t m_DUBIndex;
	Transform m_Transform;
	Mesh* m_Mesh = nullptr;
	Material* m_Material = nullptr;

	uint32_t m_NumFramesDirty = FrameResourcesCount;
};