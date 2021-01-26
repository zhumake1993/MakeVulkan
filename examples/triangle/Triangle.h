#pragma once

#include "Example.h"

class Triangle : public Example
{

public:

	Triangle();
	~Triangle();

	void ConfigDeviceProperties() override;
	void Init() override;
	void Release() override;
	void Update() override;

private:

	void PrepareResources();

private:

	// Mesh
	Mesh* m_CubeMesh;
	Mesh* m_CustomCubeMesh;
	Mesh* m_SphereMesh;

	// Texture
	Texture* m_Crate01Tex;
	Texture* m_Crate02Tex;
	Texture* m_MetalplateTex;
};