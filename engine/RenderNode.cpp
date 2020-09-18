#include "RenderNode.h"
#include "Mesh.h"
#include "VulkanBuffer.h"

VulkanBuffer * RenderNode::GetVertexBuffer()
{
	return m_Mesh->GetVertexBuffer();
}

VulkanBuffer * RenderNode::GetIndexBuffer()
{
	return m_Mesh->GetIndexBuffer();
}

uint32_t RenderNode::GetIndexCount()
{
	return m_Mesh->GetIndexCount();
}
