#pragma once

#include "NonCopyable.h"
#include <vector>

class Mesh;
class Texture;
class Shader;
class Material;
class RenderNode;

class RendererScene : public NonCopyable
{
public:

	RendererScene();
	virtual ~RendererScene();

	void BindGlobalUniformBuffer(void * data, uint64_t size);
	void BindPerViewUniformBuffer(void * data, uint64_t size);

	void SetShader(Shader* shader);

	void BindMaterial(Material* material);

	void DrawRenderNode(RenderNode* node);

	// 要求相同Material
	void DrawBatch(std::vector<RenderNode*> nodes);

	// Instancing
	void DrawInstanced(Mesh* mesh, Shader* shader, void* data, uint64_t size, uint32_t instanceCount);

private:

	// todo
	Shader* m_DummyShader;
};