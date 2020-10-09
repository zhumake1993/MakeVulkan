#include "RenderNode.h"
#include "Mesh.h"
#include "VKBuffer.h"

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
