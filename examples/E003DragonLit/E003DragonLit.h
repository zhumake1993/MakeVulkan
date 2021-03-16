#pragma once

#include "Example.h"
#include "Transform.h"

struct Light
{
	alignas(16) glm::vec3 strength; // light color
	float falloffStart; // point/spot light only
	alignas(16) glm::vec3 direction;// directional/spot lightonly
	float falloffEnd; // point/spot light only
	alignas(16) glm::vec3 position; // point/spot light only
	float spotPower; // spot light only
};

struct UniformDataGlobal
{
	alignas(16) glm::vec4 time;
};

struct UniformDataPerView
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;

	alignas(16) glm::vec4 ambientLight;
	alignas(16) Light lights[16];
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

	Transform m_PointLightTransform; // 并不需要
};