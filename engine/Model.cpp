#include "Model.h"
#include "Tools.h"
#include "obj/tiny_obj_loader.h"
#include <sstream>

Model::Model()
{
}

Model::~Model()
{
}

bool Model::loadFromFile(const std::string & filename)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

#if defined(_WIN32)

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
		LOG("LoadObj failed: %s", (warn + err).c_str());
		return false;
	}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

	// �ֻ���Ҫ����ר�ŵĺ��������ݶ�ȡ����
	std::istringstream ss(GetBinaryFileContents(filename).data());

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &ss)) {
		LOG("LoadObj failed: %s", (warn + err).c_str());
		return false;
	}

#endif

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {

			// pos
			m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
			m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
			m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
			m_Vertices.push_back(1.0f);

			// color
			m_Vertices.push_back(0.0f);
			m_Vertices.push_back(0.0f);
			m_Vertices.push_back(0.0f);
			m_Vertices.push_back(0.0f);

			// uv
			// y����Ҫ��ת
			m_Vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
			m_Vertices.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);

			m_Indices.push_back(static_cast<uint32_t>(m_Indices.size()));
		}
	}

	return true;
}
