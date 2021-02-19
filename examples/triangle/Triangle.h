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
	Mesh* m_QuadMesh;

	// Texture
	Texture* m_Crate01Tex;
	Texture* m_Crate02Tex;
	Texture* m_MetalplateTex;
	Texture* m_HomeTex;

	// Shader
	Shader* m_ColorShader;
	Shader* m_TexShader;
	Shader* m_LitShader;
	Shader* m_TestMipShader;

	// Material
	Material* m_ColorMat;
	Material* m_TexMat;
	Material* m_LitMat;
	Material* m_HomeMat;
	Material* m_TestMipMat;

	// RenderNode
	RenderNode* m_ColorCubeNode;
	RenderNode* m_TexCubeNode;
	RenderNode* m_LitSphereNode;
	RenderNode* m_HomeNode;
	RenderNode* m_TestMipNode;

	// Camera
	Camera* m_Camera;

	//
	float m_LodBias = 0;
};