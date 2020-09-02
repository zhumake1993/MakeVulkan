#include "Mesh.h"
#include "Tools.h"
#include "obj/tiny_obj_loader.h"
#include <sstream>
#include "VulkanDriver.h"
#include "VulkanBuffer.h"

VkFormat VertexChannelToVkFormat[kVertexChannelCount] = {
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32_SFLOAT
};

Mesh::Mesh()
{
	m_VertexChannels.resize(kVertexChannelCount);
}

Mesh::~Mesh()
{
	RELEASE(m_VertexBuffer);
	RELEASE(m_IndexBuffer);
}

void Mesh::SetVertexChannels(std::vector<VertexChannel>& channels)
{
	for (auto& channel : channels) {
		m_VertexChannels[channel] = true;
	}
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

			if (m_VertexChannels[kVertexPosition]) {
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
			}
			
			if (m_VertexChannels[kVertexNormal]) {
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
				m_Vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
			}

			if (m_VertexChannels[kVertexColor]) {
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 0]);
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 1]);
				m_Vertices.push_back(attrib.colors[3 * index.vertex_index + 2]);
			}

			if (m_VertexChannels[kVertexTexcoord]) {
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

void Mesh::UploadToGPU()
{
	auto driver = GetVulkanDriver();

	// Vertex buffer
	uint32_t vertexBufferSize = static_cast<uint32_t>(m_Vertices.size()) * sizeof(m_Vertices[0]);
	m_VertexBuffer = driver.CreateVulkanBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	driver.UploadVulkanBuffer(m_VertexBuffer, m_Vertices.data(), vertexBufferSize);

	// Index buffer
	uint32_t indexBufferSize = static_cast<uint32_t>(m_Indices.size()) * sizeof(uint32_t);
	m_IndexBuffer = driver.CreateVulkanBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	driver.UploadVulkanBuffer(m_IndexBuffer, m_Indices.data(), indexBufferSize);
}

std::vector<VkFormat> Mesh::GetVertexFormats()
{
	std::vector<VkFormat> vertexFormats;

	for (int i = 0; i < kVertexChannelCount; i++) {
		if (m_VertexChannels[i]) {
			vertexFormats.push_back(VertexChannelToVkFormat(static_cast<VertexChannel>(i)));
		}
	}

	return vertexFormats;
}

VulkanBuffer * Mesh::GetVertexBuffer()
{
	return m_VertexBuffer;
}

VulkanBuffer * Mesh::GetIndexBuffer()
{
	return m_IndexBuffer;
}

uint32_t Mesh::GetIndexCount()
{
	return static_cast<uint32_t>(m_Indices.size());
}

VkFormat Mesh::VertexChannelToVkFormat(VertexChannel channel)
{
	switch (channel)
	{
	case kVertexPosition:
		return VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case kVertexNormal:
		return VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case kVertexColor:
		return VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case kVertexTexcoord:
		return VK_FORMAT_R32G32_SFLOAT;
		break;
	default:
		LOG("wrong vertex channel");
		assert(false);
		return VK_FORMAT_UNDEFINED;
	}
}
