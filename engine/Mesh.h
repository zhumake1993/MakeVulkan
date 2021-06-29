#pragma once

#include "GfxTypes.h"
#include "mkString.h"
#include <vector>

class GfxBuffer;

class Mesh
{
public:

	Mesh(const mkString& name);
	virtual ~Mesh();

	void SetVertexChannels(const std::vector<VertexChannel>& channels);
	std::vector<VertexChannel>& GetVertexChannels();

	VertexDescription* GetVertexDescription();

	void LoadFromFile(const mkString& filename);

	void SetVertices(const std::vector<float>& vertices);
	void SetIndices(const std::vector<uint32_t>& indices);

	void UploadToGPU();

	GfxBuffer* GetVertexBuffer();
	GfxBuffer* GetIndexBuffer();
	uint32_t GetIndexCount();
	VkIndexType GetIndexType();

private:

	bool HasVertexChannel(VertexChannel channel);

	void LoadUseObj(const mkString& filename);
	void LoadUseAssimp(const mkString& filename);

private:

	mkString m_Name;

	std::vector<VertexChannel> m_VertexChannels;

	VertexDescription m_VertexDescription;

	std::vector<float> m_Vertices;
	std::vector<uint32_t> m_Indices;

	GfxBuffer* m_VertexBuffer = nullptr;
	GfxBuffer* m_IndexBuffer = nullptr;

	VkIndexType m_IndexType = VK_INDEX_TYPE_UINT32;
};