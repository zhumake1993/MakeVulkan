#pragma once

#include "Transform.h"
#include "mkString.h"

class Mesh;
class Material;

class RenderNode 
{

public:

	RenderNode(const mkString& name);
	virtual ~RenderNode();

	Transform& GetTransform();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();

	void SetMaterial(Material* material);
	Material* GetMaterial();

private:

	mkString m_Name;

	Transform m_Transform;

	Mesh* m_Mesh = nullptr;
	Material* m_Material = nullptr;
};