#pragma once

#include "Example.h"

struct UniformDataGlobal
{
};

struct UniformDataPerView
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;
	alignas(16) glm::vec4 ambientLight;
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

	Mesh* m_Mesh;
	Texture* m_Tex;
	Shader* m_Shader;
	Material* m_Mat;
	RenderNode* m_Node;

	Camera* m_Camera;

	int m_TextureIndex = 0;
};