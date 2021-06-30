#pragma once

#include "NamedObject.h"
#include "Transform.h"

class Mesh;
class Material;

class RenderNode : public NamedObject
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

	Transform m_Transform;

	Mesh* m_Mesh = nullptr;
	Material* m_Material = nullptr;
};