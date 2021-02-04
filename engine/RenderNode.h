#pragma once

#include "Env.h"
#include "Transform.h"

class Mesh;
class Material;

class RenderNode 
{

public:

	RenderNode(std::string name);
	virtual ~RenderNode();

	Transform& GetTransform();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();

	void SetMaterial(Material* material);
	Material* GetMaterial();

	//void SetWorldMatrix(glm::mat4& world);
	//glm::mat4& GetWorldMatrix();



	

	//VKBuffer* GetVertexBuffer();
	//VKBuffer* GetIndexBuffer();
	//uint32_t GetIndexCount();

	//void SetDirty();
	//bool IsDirty();
	//void Clean();

private:

	std::string m_Name;

	//uint32_t m_DUBIndex;
	Transform m_Transform;

	Mesh* m_Mesh = nullptr;
	Material* m_Material = nullptr;

	//uint32_t m_NumFramesDirty = FrameResourcesCount;
};