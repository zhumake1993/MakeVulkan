#include "RenderNode.h"
#include "Mesh.h"
#include "Material.h"

RenderNode::RenderNode(const std::string& name):
	m_Name(name)
{
}

RenderNode::~RenderNode()
{
}

Transform & RenderNode::GetTransform()
{
	return m_Transform;
}

void RenderNode::SetMesh(Mesh * mesh)
{
	m_Mesh = mesh;
}

Mesh * RenderNode::GetMesh()
{
	return m_Mesh;
}

void RenderNode::SetMaterial(Material * material)
{
	m_Material = material;
}

Material * RenderNode::GetMaterial()
{
	return m_Material;
}