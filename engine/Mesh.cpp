#include "Mesh.h"
#include "Tools.h"
#include "GfxDevice.h"
#include "Buffer.h"
#include <sstream>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "obj/tiny_obj_loader.h"

Mesh::Mesh(std::string name) :
	m_Name(name)
{
	m_VertexChannels = {
		kVertexPosition,
		kVertexNormal,
		kVertexTexcoord0
	};
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

	m_IsVertexDescriptionCached = false;
}

std::vector<VertexChannel>& Mesh::GetVertexChannels()
{
	return m_VertexChannels;
}

VertexDescription& Mesh::GetVertexDescription()
{
	if (m_IsVertexDescriptionCached) {
		return m_VertexDescriptionCache;
	}

	uint32_t offset = 0;
	for (auto channel : m_VertexChannels) {
		m_VertexDescriptionCache.formats.push_back(VertexChannelToFormat(channel));
		m_VertexDescriptionCache.offsets.push_back(offset);
		offset += VkFormatToSize(VertexChannelToFormat(channel));
	}
	m_VertexDescriptionCache.stride = offset;

	m_IsVertexDescriptionCached = true;

	return m_VertexDescriptionCache;
}

void Mesh::LoadFromFile(const std::string & filename)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

#if defined(_WIN32)

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
		LOG("LoadObj failed: %s", (warn + err).c_str());
		assert(false);
	}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	// 手机上要先用专门的函数把数据读取出来
	std::istringstream ss(GetBinaryFileContents(filename).data());

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &ss)) {
		LOG("LoadObj failed: %s", (warn + err).c_str());
		assert(false);
	}

#endif

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {

			if (HasVertexChannel(kVertexPosition)) {
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
			}
			
			if (HasVertexChannel(kVertexNormal)) {
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
			}

			if (HasVertexChannel(kVertexColor)) {
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 0]);
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 1]);
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 2]);
			}

			if (HasVertexChannel(kVertexTexcoord0)) {
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
	m_VertexBuffer = device.CreateBuffer(kBufferTypeVertex, vertexBufferSize);
	device.UpdateBuffer(m_VertexBuffer, m_Vertices.data(), vertexBufferSize);

	// Index buffer
	uint64_t indexBufferSize = static_cast<uint64_t>(m_Indices.size()) * sizeof(uint32_t);
	m_IndexBuffer = device.CreateBuffer(kBufferTypeIndex, indexBufferSize);
	device.UpdateBuffer(m_IndexBuffer, m_Indices.data(), indexBufferSize);
}

bool Mesh::HasVertexChannel(VertexChannel channel)
{
	return std::find(m_VertexChannels.begin(), m_VertexChannels.end(), channel) != m_VertexChannels.end();
}
