#pragma once

#include "Common.h"
#include "VKTypes.h"

struct VKBuffer;

class Mesh
{

public:

	Mesh();
	~Mesh();

	void SetVertexChannels(const std::vector<VertexChannel>& channels);
	std::vector<VertexChannel>& GetVertexChannels();

	std::vector<VkFormat>& GetVertexChannelFormats();

	VertexDescription GetVertexDescription();

	void LoadFromFile(const std::string& filename);

	void SetVertices(const std::vector<float>& vertices);
	void SetIndices(const std::vector<uint32_t>& indices);

	void UploadToGPU();

	VKBuffer* GetVertexBuffer();
	VKBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();

private:

	bool HasVertexChannel(VertexChannel channel);

public:

	//

private:

	std::vector<VertexChannel> m_VertexChannels;
	std::vector<VkFormat> m_VertexChannelFormats;

	std::vector<float> m_Vertices;
	std::vector<uint32_t> m_Indices;

	VKBuffer* m_VertexBuffer;
	VKBuffer* m_IndexBuffer;
};