#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "EngineFwd.h"
#include "DescriptorSetTypes.h"

class Imgui;
class GPUProfilerMgr;

struct Light
{
	alignas(16) glm::vec3 strength; // light color
				float falloffStart; // point/spot light only
	alignas(16) glm::vec3 direction;// directional/spot lightonly
				float falloffEnd; // point/spot light only
	alignas(16) glm::vec3 position; // point/spot light only
				float spotPower; // spot light only
};

struct PassUniform {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 eyePos;

	alignas(16) glm::vec4 ambientLight;
	alignas(16) Light lights[16];
};

struct ObjectUniform {
	alignas(16) glm::mat4 world;
};

struct MaterialUniform {
	alignas(16) glm::vec4 diffuseAlbedo;
	alignas(16) glm::vec3 fresnelR0;
				float roughness;
	alignas(16) glm::mat4 matTransform = glm::mat4(1.0f);
};

class Engine : public NonCopyable
{

public:

	Engine();
	virtual ~Engine();

	void CleanUpEngine();
	void InitEngine();
	void TickEngine();

protected:

	virtual void CleanUp() = 0;
	virtual void Init() = 0;
	virtual void Tick() = 0;
	virtual void TickUI() = 0;
	virtual void RecordCommandBuffer(VKCommandBuffer* vkCommandBuffer) = 0;

	VKFramebuffer* RebuildFramebuffer(VKRenderPass* vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);

	void UpdatePassUniformBuffer(void* data);
	void UpdateObjectUniformBuffer(RenderNode* node);
	void UpdateMaterialUniformBuffer(Material* material);

	VKBuffer* GetCurrPassUniformBuffer();
	VKBuffer* GetCurrObjectUniformBuffer();
	VKBuffer* GetCurrMaterialUniformBuffer();

	uint32_t GetObjectUBODynamicAlignment();
	uint32_t GetMaterialUBODynamicAlignment();

	Mesh* CreateMesh();
	Texture* CreateTexture();
	Shader* CreateShader();
	Material* CreateMaterial();
	RenderNode* CreateRenderNode();

private:

	void WaitForPresent();
	void Present();

protected:

	Imgui* m_Imgui;
	GPUProfilerMgr *m_GPUProfilerMgr;

private:

	struct FrameResource {
		VKFramebuffer* framebuffer;
		VKCommandBuffer* commandBuffer;
		VKSemaphore* imageAvailableSemaphore;
		VKSemaphore* finishedRenderingSemaphore;
		VKFence* fence;

		FrameResource() :
			framebuffer(nullptr),
			commandBuffer(nullptr),
			imageAvailableSemaphore(nullptr),
			finishedRenderingSemaphore(nullptr),
			fence(nullptr) {
		}
	};

	VKCommandPool* m_VKCommandPool;

	uint32_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	std::vector<VKBuffer*> m_PassUniformBuffers;

	const uint32_t m_ObjectUniformNum = MaxObjectsCount;
	std::vector<VKBuffer*> m_ObjectUniformBuffers;
	uint32_t m_ObjectUBODynamicAlignment;

	const uint32_t m_MaterialUniformNum = MaxMaterialsCount;
	std::vector<VKBuffer*> m_MaterialUniformBuffers;
	uint32_t m_MaterialUBODynamicAlignment;

	std::vector<Mesh*> m_MeshContainer;
	std::vector<Texture*> m_TextureContainer;
	std::vector<Shader*> m_ShaderContainer;
	std::vector<Material*> m_MaterialContainer;
	std::vector<RenderNode*> m_RenderNodeContainer;
};