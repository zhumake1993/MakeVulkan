#pragma once

#include "Common.h"
#include "VKTypes.h"

struct VKBuffer;

class Mesh
{

public:

	Mesh();
	~Mesh();

	std::vector<VkFormat>& GetVertexChannelFormats();
	VertexDescription GetVertexDescription(const std::vector<VertexChannel>& channels);

	void LoadFromFile(const std::string& filename);

	void UploadToGPU();

	VKBuffer* GetVertexBuffer();
	VKBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();

private:

	//

public:

	//

private:

	std::vector<VkFormat> m_VertexChannelFormats;

	std::vector<float> m_Vertices;
	std::vector<uint32_t> m_Indices;

	VKBuffer* m_VertexBuffer;
	VKBuffer* m_IndexBuffer;
};