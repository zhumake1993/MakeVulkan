#pragma once

#include "Env.h" 
#include "GfxTypes.h"
#include "imgui/imgui.h"

class Image;
class Buffer;
class Shader;

class Imgui
{

public:

	Imgui();
	~Imgui();

	void Prepare(float deltaTime);
	void Tick();
	void Draw();

private:

	//

public:

	//

private:

	Image* m_FontImage = nullptr;

	const uint32_t m_MaxVertexCount = 65536;
	const uint32_t m_MaxIndexCount = 65536;
	Buffer* m_VertexBuffer = nullptr;
	Buffer* m_IndexBuffer = nullptr;

	Shader* m_Shader;

	VertexDescription m_VertexDes;
};