#pragma once

#include "Engine.h"
#include "VulkanFwd.h"
#include "Camera.h"

void ConfigGlobalSettings();

class Triangle : public Engine
{
	struct UniformBuffer
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

public:

	Triangle();
	virtual ~Triangle();

protected:

	void CleanUp() override;
	void Init() override;
	void Tick() override;
	void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer, VulkanFramebuffer* vulkanFramebuffer) override;

private:

	void PrepareVertices();
	void PrepareTextures();
	void PrepareDescriptorSet();
	void CreatePipeline();

private:

	VulkanPipeline* m_VulkanPipeline;
	VulkanPipelineLayout* m_VulkanPipelineLayout;

	VulkanBuffer* m_VertexBuffer;
	VulkanBuffer* m_IndexBuffer;
	VulkanImage* m_Image;

	VulkanDescriptorPool* m_VulkanDescriptorPool;
	VulkanDescriptorSetLayout* m_VulkanDescriptorSetLayout;
	VulkanDescriptorSet* m_VulkanDescriptorSet;



	UniformBuffer m_UniformBuffer;

	// ÉãÏñ»ú
	Camera m_Camera;

	// stats
	uint32_t m_FrameIndex = 0;
	uint32_t m_AccumulateCounter = 0;
	float m_AccumulateTime = 0;
	float m_FPS = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp = std::chrono::high_resolution_clock::now();
};