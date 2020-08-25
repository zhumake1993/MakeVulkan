#pragma once

#include "Common.h"

class Model
{

public:

	Model();
	~Model();

	bool loadFromFile(const std::string& filename);

private:

	//

public:

	std::vector<float> m_Vertices;
	std::vector<uint32_t> m_Indices;

private:

// 	std::vector<float> m_Vertices;
// 	std::vector<uint32_t> m_Indices;
};