#pragma once

#include "Env.h" 
#include "imgui/imgui.h"

class Image;
class Buffer;
class Shader;

class Imgui
{

public:

	Imgui();
	~Imgui();

	void Prepare();
	void Tick();
	void RecordCommandBuffer(VKCommandBuffer* vkCommandBuffer);

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

	//VKImage* m_FontImage;
	//VKSampler* m_Sampler;

	
	//VKBuffer* m_VertexBuffer;
	//VKBuffer* m_IndexBuffer;

	//VkDescriptorSet m_DescriptorSet;
	//VKPipeline* m_VulkanPipeline;

	//VKRenderPass* m_RenderPass;
};