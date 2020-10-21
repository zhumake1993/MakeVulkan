#pragma once

#include "Common.h"
#include "VKTypes.h"

struct VKBuffer;

class Mesh
{

public:

	Mesh();
	~Mesh();

	void SetVertexChannels(std::vector<VertexChannel>& channels);
	std::vector<VertexChannel>& GetVertexChannels();

	std::vector<VkFormat>& GetVertexChannelFormats();

	void LoadFromFile(const std::string& filename);

	// 目前默认是一个简单的立方体，只有pos,color,tex，其中tex全为0
	//void LoadFromGeo();
	void UploadToGPU();

	VertexDescription GetVertexDescription();
	VKBuffer* GetVertexBuffer();
	VKBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();

private:

	bool HasVertexChannel(int channel);

public:

	//

private:

	// 应该放在material里，todo
	std::vector<VertexChannel> m_VertexChannels;

	// 这个恐怕不应该放在material里
	std::vector<VkFormat> m_VertexChannelFormats;

	std::vector<float> m_Vertices;
	std::vector<uint32_t> m_Indices;

	VKBuffer* m_VertexBuffer;
	VKBuffer* m_IndexBuffer;
};