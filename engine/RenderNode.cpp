#include "RenderNode.h"
#include "Mesh.h"
#include "VKBuffer.h"

RenderNode::RenderNode(uint32_t objectUBIndex):
	m_ObjectUBIndex(objectUBIndex)
{
}

RenderNode::~RenderNode()
{
}

void RenderNode::SetObjectUBIndex(uint32_t index)
{
	m_ObjectUBIndex = index;
}

uint32_t RenderNode::GetObjectUBIndex()
{
	return m_ObjectUBIndex;
}

Transform & RenderNode::GetTransform()
{
	return m_Transform;
}

void RenderNode::SetWorldMatrix(glm::mat4& world)
{
	m_Transform.SetMatrix(world);
}

glm::mat4 RenderNode::GetWorldMatrix()
{
	return m_Transform.GetMatrix();
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

void RenderNode::SetDirty()
{
	m_NumFramesDirty = FrameResourcesCount;
}

bool RenderNode::IsDirty()
{
	return m_NumFramesDirty > 0;
}

void RenderNode::Clean()
{
	if (m_NumFramesDirty > 0) {
		m_NumFramesDirty--;
	}
}
