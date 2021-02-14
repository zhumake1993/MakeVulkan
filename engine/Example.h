#pragma once

#include "Env.h"
#include "NonCopyable.h"

class Mesh;
class Texture;
class Shader;
class Material;
class RenderNode;
class Camera;
class Buffer;
class Imgui;

struct UniformDataGlobal
{
	alignas(16) glm::vec4 time;
};

struct Light
{
	alignas(16) glm::vec3 strength; // light color
	float falloffStart; // point/spot light only
	alignas(16) glm::vec3 direction;// directional/spot lightonly
	float falloffEnd; // point/spot light only
	alignas(16) glm::vec3 position; // point/spot light only
	float spotPower; // spot light only
};

struct UniformDataPerView
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;

	alignas(16) glm::vec4 ambientLight;
	alignas(16) Light lights[16];
};

class TimeManager;

class Example : public NonCopyable
{
public:

	Example();
	virtual ~Example();

	virtual void ConfigDeviceProperties() = 0;
	virtual void Init();
	virtual void Release();
	virtual void Update();
	virtual void Draw() = 0;

protected:

	Mesh* CreateMesh(const std::string& name);
	Texture* CreateTexture(const std::string& name);
	Shader* CreateShader(const std::string& name);
	Material* CreateMaterial(const std::string& name);
	RenderNode* CreateRenderNode(const std::string& name);

	void SetShader(Shader* shader);
	void BindGlobalUniformBuffer();
	void BindPerViewUniformBuffer();
	void BindMaterial(Material* material);
	void DrawRenderNode(RenderNode* node);
	void UpdateImgui();
	void DrawImgui();

protected:

	UniformDataGlobal m_UniformDataGlobal;
	UniformDataPerView m_UniformDataPerView;
	
	TimeManager* m_TimeManager;

private:

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;

	Shader* m_DummyShader;
	Imgui* m_Imgui = nullptr;
};