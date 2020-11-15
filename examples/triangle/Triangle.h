#pragma once

#include "Engine.h"
#include "VulkanFwd.h"

class Triangle : public Engine
{

public:

	Triangle();
	virtual ~Triangle();

protected:

	void CleanUp() override;
	void Init() override;
	void Tick() override;
	void TickUI() override;
	void RecordCommandBuffer(VKCommandBuffer* vkCommandBuffer) override;

private:

	void PrepareResources();

private:

	// Test
	float m_Temp=0;

	// Mesh
	Mesh* m_Home;
	Mesh* m_Cube;
	Mesh* m_SimpleCube;
	Mesh* m_Sphere;
	Mesh* m_Quad;

	// Texture
	Texture* m_HomeTex;
	Texture* m_Crate01Tex;
	Texture* m_Crate02Tex;
	Texture* m_Metalplate;

	// Shader
	Shader* m_Shader;
	Shader* m_SimpleShader;

	// Material
	Material* m_HomeMat;
	Material* m_Crate01Mat;
	Material* m_Crate02Mat;
	Material* m_SimpleColorMat;
	Material* m_MetalplateMat;

	// RenderNode
	RenderNode* m_HomeNode;
	RenderNode* m_CubeNode1;
	RenderNode* m_SphereNode;
	RenderNode* m_ColorCubeNode;
	RenderNode* m_QuadNode;

	// camera
	Camera* m_Camera;
};