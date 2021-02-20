#pragma once

#include "Example.h"

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

	Mesh* m_Mesh;
	Texture* m_Tex;
	Shader* m_Shader;
	Material* m_Mat;
	RenderNode* m_Node;

	Camera* m_Camera;

	float m_LodBias = 0;
};