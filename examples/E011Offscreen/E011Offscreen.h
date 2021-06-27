#pragma once

#include "Engine.h"

#include "GLMIncludes.h"
struct UniformDataGlobal
{
};

struct UniformDataPerView
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;

	alignas(16) glm::vec4 ambient;
	alignas(16) glm::vec4 strength;
	alignas(16) glm::vec4 direction;
};

class Mesh;
class Texture;
class Attachment;
class Shader;
class Material;
class RenderNode;
class Camera;
class Buffer;
class RenderPass;

class MakeVulkan : public Engine
{

public:

	MakeVulkan();
	virtual ~MakeVulkan();

	virtual void PreInit() override;
	virtual void Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void Draw() override;

private:

	void PrepareResources();

private:

	UniformDataGlobal m_UniformDataGlobal;
	UniformDataPerView m_UniformDataPerView;

	Mesh* m_DragonMesh;

	Shader* m_PhongShader;
	Shader* m_MiniMapShader;

	Material* m_PhongMat;
	Material* m_MiniMapMat;

	RenderNode* m_DragonNode;

	RenderPass* m_RenderPassOffscreen;
	RenderPass* m_RenderPass;

	Camera* m_Camera;

	float m_ClipY = -6.0f;
};