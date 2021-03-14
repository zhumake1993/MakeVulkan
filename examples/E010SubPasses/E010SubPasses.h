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

	Mesh* m_BuildingMesh;
	Mesh* m_GlassMesh;

	Texture* m_Tex;

	Shader* m_GBufferShader;
	Shader* m_CompositionShader;

	Material* m_GBufferMat;
	Material* m_CompositionMat;

	RenderNode* m_BuildingNode;
	RenderNode* m_GlassNode;

	Camera* m_Camera;

	// 0:Position
	// 1:Color
	// 2:Normal
	int m_Mode = 0;

	int m_LightCount = 64;

	RenderPassDesc m_RenderPassDesc;
};