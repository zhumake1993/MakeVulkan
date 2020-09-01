#pragma once

#include "Common.h"

class VulkanBuffer;

enum VertexChannel {
	kVertexPosition,
	kVertexNormal,
	kVertexColor,
	kVertexTexcoord,
	//kVertexTangent,
	//kVertexBitangent,

	kVertexChannelCount
};

class Mesh
{

public:

	Mesh();
	~Mesh();

	void SetVertexChannels(std::vector<VertexChannel>& channels);
	void LoadFromFile(const std::string& filename);
	void UploadToGPU();

	std::vector<VkFormat> GetVertexFormats();
	VulkanBuffer* GetVertexBuffer();
	VulkanBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();

private:

	VkFormat VertexChannelToVkFormat(VertexChannel channel);

public:

	//

private:

	std::vector<bool> m_VertexChannels;

	std::vector<float> m_Vertices;
	std::vector<uint32_t> m_Indices;

	VulkanBuffer* m_VertexBuffer;
	VulkanBuffer* m_IndexBuffer;
};