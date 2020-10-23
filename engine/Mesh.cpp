#include "Mesh.h"
#include "Tools.h"
#include "obj/tiny_obj_loader.h"
#include <sstream>
#include <algorithm>
#include "VulkanDriver.h"
#include "VKBuffer.h"

Mesh::Mesh()
{
	m_VertexChannels = {
		kVertexPosition,
		kVertexNormal,
		kVertexColor,
		kVertexTexcoord
	};

	m_VertexChannelFormats = {
		VK_FORMAT_R32G32B32_SFLOAT, // kVertexPosition
		VK_FORMAT_R32G32B32_SFLOAT, // kVertexNormal
		VK_FORMAT_R32G32B32_SFLOAT, // kVertexColor
		VK_FORMAT_R32G32_SFLOAT // kVertexTexcoord
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
}

std::vector<VertexChannel>& Mesh::GetVertexChannels()
{
	return m_VertexChannels;
}

std::vector<VkFormat>& Mesh::GetVertexChannelFormats()
{
	return m_VertexChannelFormats;
}

VertexDescription Mesh::GetVertexDescription()
{
	VertexDescription vd;

	uint32_t offset = 0;
	for (auto channel : m_VertexChannels) {
		vd.formats.push_back(m_VertexChannelFormats[channel]);
		vd.offsets.push_back(offset);
		offset += VkFormatToSize(m_VertexChannelFormats[channel]);
	}
	vd.stride = offset;

	return vd;
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

			if (HasVertexChannel(kVertexTexcoord)) {
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

void Mesh::SetVertices(std::vector<float>& vertices)
{
	m_Vertices = vertices;
}

void Mesh::SetIndices(std::vector<uint32_t>& indices)
{
	m_Indices = indices;
}

void Mesh::UploadToGPU()
{
	auto& driver = GetVulkanDriver();

	// Vertex buffer
	uint32_t vertexBufferSize = static_cast<uint32_t>(m_Vertices.size()) * sizeof(m_Vertices[0]);
	m_VertexBuffer = driver.CreateVKBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	driver.UploadVKBuffer(m_VertexBuffer, m_Vertices.data(), vertexBufferSize);

	// Index buffer
	uint32_t indexBufferSize = static_cast<uint32_t>(m_Indices.size()) * sizeof(uint32_t);
	m_IndexBuffer = driver.CreateVKBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	driver.UploadVKBuffer(m_IndexBuffer, m_Indices.data(), indexBufferSize);
}

VKBuffer * Mesh::GetVertexBuffer()
{
	return m_VertexBuffer;
}

VKBuffer * Mesh::GetIndexBuffer()
{
	return m_IndexBuffer;
}

uint32_t Mesh::GetIndexCount()
{
	return static_cast<uint32_t>(m_Indices.size());
}

bool Mesh::HasVertexChannel(VertexChannel channel)
{
	return std::find(m_VertexChannels.begin(), m_VertexChannels.end(), channel) != m_VertexChannels.end();
}
