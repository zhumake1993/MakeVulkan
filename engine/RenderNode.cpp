#include "RenderNode.h"
#include "Mesh.h"
#include "VKBuffer.h"

void RenderNode::SetObjectUBIndex(uint32_t index)
{
	m_ObjectUBIndex = index;
}

uint32_t RenderNode::GetObjectUBIndex()
{
	return m_ObjectUBIndex;
}

void RenderNode::SetWorldMatrix(glm::mat4 world)
{
	m_WorldMatrix = world;

	m_NumFramesDirty = FrameResourcesCount;
}

glm::mat4 RenderNode::GetWorldMatrix()
{
	return m_WorldMatrix;
}

void RenderNode::SetMesh(Mesh * mesh)
{
	m_Mesh = mesh;
}

Mesh * RenderNode::GetMesh()
{
	return m_Mesh;
}

VKBuffer * RenderNode::GetVertexBuffer()
{
	return m_Mesh->GetVertexBuffer();
}

VKBuffer * RenderNode::GetIndexBuffer()
{
	return m_Mesh->GetIndexBuffer();
}

uint32_t RenderNode::GetIndexCount()
{
	return m_Mesh->GetIndexCount();
}
