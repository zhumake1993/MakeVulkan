#pragma once

#include "Example.h"

struct UniformDataGlobal
{
};

struct UniformDataPerView
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::mat4 invView;
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

	Mesh* m_SphereMesh;
	Mesh* m_TorusMesh;
	Mesh* m_TorusknotMesh;
	Mesh* m_SkyboxMesh;

	Texture* m_Tex;

	Shader* m_ReflectShader;
	Shader* m_SkyboxShader;

	Material* m_ReflectMat;
	Material* m_SkyboxMat;

	std::vector<RenderNode*> m_Nodes;

	Camera* m_Camera;

	int m_MeshIndex = 0;
	int m_TextureIndex = 0;
	float m_LodBias = 0;
};