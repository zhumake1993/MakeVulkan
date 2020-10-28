#pragma once

#include "Common.h"
#include "NonCopyable.h"
#include "VulkanFwd.h"
#include "DescriptorSetMgr.h"

class GPUProfilerMgr;
class UniformBufferMgr;
class Engine;

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

class VulkanDriver : public NonCopyable
{

	struct FrameResource {
		// ËÆºõ²»ÐèÒªVKFramebuffer£¬todo
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

public:
	VulkanDriver();
	virtual ~VulkanDriver();

	void CleanUp();
	void Init();
	void Tick();

	// Device
	void DeviceWaitIdle();
	uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);
	VkFormat GetSupportedDepthFormat();

	// SwapChain
	//VkImageView GetSwapChainCurrImageView();
	//uint32_t GetSwapChainWidth();
	//uint32_t GetSwapChainHeight();
	VkFormat GetSwapChainFormat();

	//
	void WaitForPresent();
	void Present();

	// Command
	VKCommandPool* CreateVKCommandPool();
	VKQueryPool* CreateVKQueryPool(VkQueryType queryType, uint32_t queryCount);
	VKCommandBuffer* CreateVKCommandBuffer(VKCommandPool* vkCommandPool);

	// Semaphore
	VKSemaphore* CreateVKSemaphore();
	VKFence* CreateVKFence(bool signaled);

	// Resource
	VKBuffer* CreateVKBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty);
	VKImage* CreateVKImage(VkImageCreateInfo& imageCI, VkImageViewCreateInfo& viewCI);
	VKSampler* CreateVKSampler(VkSamplerCreateInfo& ci);
	void UploadVKBuffer(VKBuffer* vkBuffer, void* data, VkDeviceSize size);
	void UploadVKImage(VKImage* image, void* data, VkDeviceSize size);

	// DescriptorSet
	VkDescriptorSetLayout CreateDescriptorSetLayout(DSLBindings& bindings);
	VkDescriptorSet GetDescriptorSet(VkDescriptorSetLayout layout, bool persistent = false);
	void UpdateDescriptorSet(VkDescriptorSet set, DesUpdateInfos& infos);

	// GPUProfilerMgr
	void WriteTimeStamp(std::string name);
	std::string GetLastFrameGPUProfilerResult();

	// Render Status
	VKShaderModule* CreateVKShaderModule(const std::string& filename);
	VKPipelineLayout* CreateVKPipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts, VkShaderStageFlags pcStage = VK_SHADER_STAGE_VERTEX_BIT, uint32_t pcSize = 0);
	VKPipeline* CreateVKPipeline(PipelineCI& pipelineCI, VKPipeline* parent = nullptr);
	VKRenderPass* CreateVKRenderPass(VkFormat colorFormat, VkFormat depthFormat);

	// Framebuffer
	VKFramebuffer* CreateVKFramebuffer(VKRenderPass* vkRenderPass, VkImageView color, VkImageView depth, uint32_t width, uint32_t height);
	VKFramebuffer* RebuildFramebuffer(VKRenderPass* vkRenderPass);

	// todo
	VKBuffer* GetCurrPassUniformBuffer();
	VKBuffer* GetCurrObjectUniformBuffer();
	VKBuffer* GetCurrMaterialUniformBuffer();

	uint32_t GetObjectUBODynamicAlignment();
	uint32_t GetMaterialUBODynamicAlignment();

private:

	void AcquireNextImage(VKSemaphore* vkSemaphore);

	void QueuePresent(VKSemaphore* vkSemaphore);

private:

	// base
	VKInstance* m_VKInstance;
	VKSurface* m_VKSurface;
	VKDevice* m_VKDevice;
	VKSwapChain* m_VKSwapChain;
	VKCommandPool* m_VKCommandPool;

	// frame resources
	uint32_t m_CurrFrameIndex = 0;
	std::vector<FrameResource> m_FrameResources;

	// uniform buffer
	UniformBufferMgr* m_UniformBufferMgr;

	//std::vector<VKBuffer*> m_PassUniformBuffers;

	//const uint32_t m_ObjectUniformNum = MaxObjectsCount;
	//std::vector<VKBuffer*> m_ObjectUniformBuffers;
	uint32_t m_ObjectUBODynamicAlignment;

	//const uint32_t m_MaterialUniformNum = MaxMaterialsCount;
	//std::vector<VKBuffer*> m_MaterialUniformBuffers;
	uint32_t m_MaterialUBODynamicAlignment;

	VKCommandBuffer* m_UploadVKCommandBuffer;
	const uint32_t m_StagingBufferSize = 10000000;
	VKBuffer* m_StagingBuffer;

	DescriptorSetMgr* m_DescriptorSetMgr;

	

	GPUProfilerMgr *m_GPUProfilerMgr;

	uint32_t m_ImageIndex;

	// depth
	VkFormat m_DepthFormat;
	VKImage* m_DepthImage;
};

void CreateVulkanDriver();
VulkanDriver& GetVulkanDriver();
void ReleaseVulkanDriver();