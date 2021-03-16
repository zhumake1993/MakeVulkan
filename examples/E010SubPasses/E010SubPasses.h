#pragma once

#include "Example.h"
#include "GfxDeviceObjects.h"

struct UniformDataGlobal
{
};

struct Light
{
	alignas(16) glm::vec4 position;
	alignas(16) glm::vec3 color;
	float radius;
};

struct UniformDataPerView
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;

	alignas(16) glm::vec4 ambient;
	alignas(16) Light lights[128];
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

	Texture* m_GlassTex;

	Shader* m_GBufferShader;
	Shader* m_CompositionShader;
	Shader* m_TransparentShader;

	Material* m_GBufferMat;
	Material* m_CompositionMat;
	Material* m_TransparentMat;

	RenderNode* m_BuildingNode;
	RenderNode* m_GlassNode;

	Camera* m_Camera;

	const int m_LightCount = 64; // 暂时不支持动态调整光源数量

	RenderPassDesc m_RenderPassDesc;
};