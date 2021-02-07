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

private:

	void Draw();
	void PrepareResources();

private:

	// Mesh
	Mesh* m_CustomCubeMesh;
	Mesh* m_TexCubeMesh;
	//Mesh* m_SphereMesh;

	// Texture
	Texture* m_Crate01Tex;
	//Texture* m_Crate02Tex;
	//Texture* m_MetalplateTex;

	// Shader
	Shader* m_ColorShader;
	Shader* m_TexShader;
	//Shader* m_LitShader;

	// Material
	Material* m_ColorMat;
	Material* m_TexMat;
	//Material* m_Crate02Mat;
	//Material* m_MetalplateMat;

	// RenderNode
	RenderNode* m_ColorCubeNode;
	RenderNode* m_TexCubeNode;
	//RenderNode* m_SphereNode;

	// Camera
	Camera* m_Camera;
};