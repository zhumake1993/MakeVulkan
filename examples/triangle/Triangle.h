#pragma once

#include "Example.h"

class Triangle : public Example
{

public:

	Triangle();
	virtual ~Triangle();

	void ConfigDeviceProperties() override;
	void Init() override;
	void Release() override;
	void Update() override;
	void Draw() override;

private:

	void PrepareResources();

private:

	// Mesh
	Mesh* m_CustomCubeMesh;
	Mesh* m_TexCubeMesh;
	Mesh* m_CubeMesh;
	Mesh* m_SphereMesh;
	Mesh* m_HomeMesh;

	// Texture
	Texture* m_Crate01Tex;
	Texture* m_Crate02Tex;
	Texture* m_MetalplateTex;
	Texture* m_HomeTex;

	// Shader
	Shader* m_ColorShader;
	Shader* m_TexShader;
	Shader* m_LitShader;

	// Material
	Material* m_ColorMat;
	Material* m_TexMat;
	Material* m_LitMat;
	Material* m_HomeMat;

	// RenderNode
	RenderNode* m_ColorCubeNode;
	RenderNode* m_TexCubeNode;
	RenderNode* m_LitSphereNode;
	RenderNode* m_HomeNode;

	// Camera
	Camera* m_Camera;
};