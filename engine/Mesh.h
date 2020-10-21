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

	// ĿǰĬ����һ���򵥵������壬ֻ��pos,color,tex������texȫΪ0
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

	// Ӧ�÷���material�todo
	std::vector<VertexChannel> m_VertexChannels;

	// ������²�Ӧ�÷���material��
	std::vector<VkFormat> m_VertexChannelFormats;

	std::vector<float> m_Vertices;
	std::vector<uint32_t> m_Indices;

	VKBuffer* m_VertexBuffer;
	VKBuffer* m_IndexBuffer;
};