#pragma once

#include "Env.h"
#include "Transform.h"

class Mesh;
class Material;

class RenderNode 
{

public:

	RenderNode(const std::string& name);
	virtual ~RenderNode();

	Transform& GetTransform();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();

	void SetMaterial(Material* material);
	Material* GetMaterial();

private:

	std::string m_Name;

	Transform m_Transform;

	Mesh* m_Mesh = nullptr;
	Material* m_Material = nullptr;
};