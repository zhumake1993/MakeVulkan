#pragma once

#include "Example.h"

struct UniformDataGlobal
{
};

struct UniformDataPerView
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class MakeVulkan : public Example
{

public:

	MakeVulkan();
	virtual ~MakeVulkan();

	void ConfigDeviceProperties() override;
	void Init() override;
	void Release() override;
	void Update() override;
	void Draw() override;

private:

	void PrepareResources();

private:

	UniformDataGlobal m_UniformDataGlobal;
	UniformDataPerView m_UniformDataPerView;

	Mesh* m_CubeMesh;
	Mesh* m_QuadMesh;
	Shader* m_Shader;
	Material* m_Mat;
	std::vector<RenderNode*> m_Nodes;

	Camera* m_Camera;

	bool m_EnableBatch = false;
};