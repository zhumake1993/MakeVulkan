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
	alignas(16) glm::vec4 eyePos;

	alignas(16) glm::vec4 ambient;
	alignas(16) glm::vec4 strength;
	alignas(16) glm::vec4 direction;
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

	Mesh* m_DragonMesh;
	Mesh* m_PlaneMesh;

	Shader* m_MirrorPhongShader;
	Shader* m_PhongShader;

	Material* m_MirrorPhongMat;
	Material* m_PhongMat;

	RenderNode* m_MirrorDragonNode;
	RenderNode* m_DragonNode;
	RenderNode* m_PlaneNode;

	Camera* m_Camera;

	float m_ClipY = 0.0f;

	RenderPassDesc m_RenderPassOffscreen;
};