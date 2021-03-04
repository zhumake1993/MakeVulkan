#pragma once

#include "Example.h"
#include "GfxDeviceObjects.h"

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

	Mesh* m_Mesh;
	Shader* m_Shader;
	Shader* m_FetchShader;
	Material* m_Mat;
	Material* m_FetchMat;
	RenderNode* m_Node;

	Camera* m_Camera;

	RenderPassDesc m_RenderPassDesc;

	bool m_FetchDepth = false;
	float m_Brightness = 0.5;
	float m_Contrast = 1.8f;
	float m_DepthMin = 0.0f;
	float m_DepthMax = 1.0f;
};