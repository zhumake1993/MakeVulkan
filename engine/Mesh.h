#pragma once

#include "GfxTypes.h"
#include "mkString.h"
#include "mkVector.h"

class GfxBuffer;

class Mesh
{
public:

	Mesh(const mkString& name);
	virtual ~Mesh();

	void SetVertexChannels(const mkVector<VertexChannel>& channels);
	mkVector<VertexChannel>& GetVertexChannels();

	VertexDescription* GetVertexDescription();

	void LoadFromFile(const mkString& filename);

	void SetVertices(const mkVector<float>& vertices);
	void SetIndices(const mkVector<uint32_t>& indices);

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

	mkVector<VertexChannel> m_VertexChannels;

	VertexDescription m_VertexDescription;

	mkVector<float> m_Vertices;
	mkVector<uint32_t> m_Indices;

	GfxBuffer* m_VertexBuffer = nullptr;
	GfxBuffer* m_IndexBuffer = nullptr;

	VkIndexType m_IndexType = VK_INDEX_TYPE_UINT32;
};