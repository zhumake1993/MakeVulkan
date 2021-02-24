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

struct InstanceData
{
	alignas(16) glm::mat4 model;
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

	Mesh* m_Mesh;
	Texture* m_Tex;
	Shader* m_Shader;
	Material* m_Mat;

	Camera* m_Camera;

	int m_InstanceSize = 0;
	InstanceData* m_InstanceData = nullptr;
};