#include "Mesh.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "Buffer.h"
#include <sstream>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "obj/tiny_obj_loader.h"

// assimp
//#include "assimp/Importer.hpp" 
//#include "assimp/scene.h"   
//#include "assimp/postprocess.h"
//#include "assimp/cimport.h"

Mesh::Mesh(const std::string& name) :
	m_Name(name)
{
	m_VertexChannels = {
		kVertexPosition,
		kVertexNormal,
		kVertexTexcoord0
	};

	uint32_t offset = 0;
	for (auto channel : m_VertexChannels)
	{
		m_VertexDescription.formats.push_back(VertexChannelToFormat(channel));
		m_VertexDescription.offsets.push_back(offset);
		offset += VkFormatToSize(VertexChannelToFormat(channel));
	}
	m_VertexDescription.stride = offset;
}

Mesh::~Mesh()
{
	RELEASE(m_VertexBuffer);
	RELEASE(m_IndexBuffer);
}

void Mesh::SetVertexChannels(const std::vector<VertexChannel>& channels)
{
	m_VertexChannels = channels;

	// 保证channel的顺序
	std::sort(m_VertexChannels.begin(), m_VertexChannels.end());

	m_VertexDescription.formats.clear();
	m_VertexDescription.offsets.clear();
	m_VertexDescription.stride = 0;

	uint32_t offset = 0;
	for (auto channel : m_VertexChannels)
	{
		m_VertexDescription.formats.push_back(VertexChannelToFormat(channel));
		m_VertexDescription.offsets.push_back(offset);
		offset += VkFormatToSize(VertexChannelToFormat(channel));
	}
	m_VertexDescription.stride = offset;
}

std::vector<VertexChannel>& Mesh::GetVertexChannels()
{
	return m_VertexChannels;
}

VertexDescription* Mesh::GetVertexDescription()
{
	return &m_VertexDescription;
}

void Mesh::LoadFromFile(const std::string & filename)
{
	LoadUseObj(filename);
	//LoadUseAssimp(filename);
}

void Mesh::SetVertices(const std::vector<float>& vertices)
{
	m_Vertices = vertices;
}

void Mesh::SetIndices(const std::vector<uint32_t>& indices)
{
	m_Indices = indices;
}

void Mesh::UploadToGPU()
{
	auto& device = GetGfxDevice();

	// Vertex buffer
	uint64_t vertexBufferSize = static_cast<uint64_t>(m_Vertices.size()) * sizeof(m_Vertices[0]);
	m_VertexBuffer = device.CreateBuffer(kBufferUsageVertex, kMemoryPropertyDeviceLocal, vertexBufferSize);
	device.UpdateBuffer(m_VertexBuffer, m_Vertices.data(), 0, vertexBufferSize);

	// Index buffer
	uint64_t indexBufferSize = static_cast<uint64_t>(m_Indices.size()) * sizeof(uint32_t);
	m_IndexBuffer = device.CreateBuffer(kBufferUsageIndex, kMemoryPropertyDeviceLocal, indexBufferSize);
	device.UpdateBuffer(m_IndexBuffer, m_Indices.data(), 0, indexBufferSize);
}

Buffer * Mesh::GetVertexBuffer()
{
	return m_VertexBuffer;
}

Buffer * Mesh::GetIndexBuffer()
{
	return m_IndexBuffer;
}

uint32_t Mesh::GetIndexCount()
{
	return static_cast<uint32_t>(m_Indices.size());
}

VkIndexType Mesh::GetIndexType()
{
	return m_IndexType;
}

bool Mesh::HasVertexChannel(VertexChannel channel)
{
	return std::find(m_VertexChannels.begin(), m_VertexChannels.end(), channel) != m_VertexChannels.end();
}

void Mesh::LoadUseObj(const std::string & filename)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

#if defined(_WIN32)

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str()))
	{
		LOG("LoadObj failed: %s", (warn + err).c_str());
		assert(false);
	}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	// 手机上要先用专门的函数把数据读取出来
	std::istringstream ss(GetBinaryFileContents(filename).data());

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &ss))
	{
		LOG("LoadObj failed: %s", (warn + err).c_str());
		assert(false);
	}

#endif

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{

			if (HasVertexChannel(kVertexPosition))
			{
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
			}

			if (HasVertexChannel(kVertexNormal))
			{
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
			}

			if (HasVertexChannel(kVertexColor))
			{
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 0]);
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 1]);
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 2]);
			}

			if (HasVertexChannel(kVertexTexcoord0))
			{
				// The OBJ format assumes a coordinate system where a vertical coordinate of 0 means the bottom of the image, 
				// however we've uploaded our image into Vulkan in a top to bottom orientation where 0 means the top of the image.
				// Solve this by flipping the vertical component of the texture coordinates
				m_Vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
				m_Vertices.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
			}

			m_Indices.push_back(static_cast<uint32_t>(m_Indices.size()));
		}
	}
}

void Mesh::LoadUseAssimp(const std::string & filename)
{
//	Assimp::Importer Importer;
//	const aiScene* pScene;
//
//	static const int defaultFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
//
//#if defined(__ANDROID__)
//
//	// 手机上要先用专门的函数把数据读取出来
//	std::vector<char> fileData = GetBinaryFileContents(filename);
//
//	pScene = Importer.ReadFileFromMemory(fileData.data(), fileData.size(), defaultFlags);
//
//#else
//
//	pScene = Importer.ReadFile(filename.c_str(), defaultFlags);
//
//#endif
//
//	if (pScene)
//	{
//		for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
//		{
//			const aiMesh* paiMesh = pScene->mMeshes[i];
//
//			aiColor3D pColor(0.f, 0.f, 0.f);
//			pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);
//
//			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
//
//			for (unsigned int j = 0; j < paiMesh->mNumVertices; j++)
//			{
//				const aiVector3D* pPos = &(paiMesh->mVertices[j]);
//				const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
//				const aiVector3D* pTexCoord = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
//				const aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;
//				const aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[j]) : &Zero3D;
//
//				for (auto& channel : m_VertexChannels)
//				{
//					switch (channel)
//					{
//					case kVertexPosition:
//						m_Vertices.push_back(pPos->x);
//						m_Vertices.push_back(-pPos->y);
//						m_Vertices.push_back(pPos->z);
//						break;
//					case kVertexNormal:
//						m_Vertices.push_back(pNormal->x);
//						m_Vertices.push_back(-pNormal->y);
//						m_Vertices.push_back(pNormal->z);
//						break;
//					case kVertexColor:
//						m_Vertices.push_back(pColor.r);
//						m_Vertices.push_back(pColor.g);
//						m_Vertices.push_back(pColor.b);
//						break;
//					case kVertexTexcoord0:
//						m_Vertices.push_back(pTexCoord->x);
//						m_Vertices.push_back(pTexCoord->y);
//						break;
//					};
//				}
//			}
//
//			for (unsigned int j = 0; j < paiMesh->mNumFaces; j++)
//			{
//				const aiFace& Face = paiMesh->mFaces[j];
//				if (Face.mNumIndices != 3)
//					continue;
//				m_Indices.push_back(Face.mIndices[0]);
//				m_Indices.push_back(Face.mIndices[1]);
//				m_Indices.push_back(Face.mIndices[2]);
//			}
//		}
//	}
//	else
//	{
//		LOGE("LoadUseAssimp failed. %s", Importer.GetErrorString());
//	}
}
