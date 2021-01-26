#pragma once

#include "Env.h"
#include "GfxTypes.h"

class Buffer;

class Mesh
{
public:

	Mesh();
	~Mesh();

	void SetVertexChannels(const std::vector<VertexChannel>& channels);
	std::vector<VertexChannel>& GetVertexChannels();

	VertexDescription& GetVertexDescription();

	void LoadFromFile(const std::string& filename);

	void SetVertices(const std::vector<float>& vertices);
	void SetIndices(const std::vector<uint32_t>& indices);

	void UploadToGPU();

private:

	bool HasVertexChannel(VertexChannel channel);

public:

	//

private:

	std::vector<VertexChannel> m_VertexChannels;

	bool m_IsVertexDescriptionCached = false;
	VertexDescription m_VertexDescriptionCache;

	std::vector<float> m_Vertices;
	std::vector<uint32_t> m_Indices;

	Buffer* m_VertexBuffer = nullptr;
	Buffer* m_IndexBuffer = nullptr;
};