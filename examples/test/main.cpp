#include <iostream>
#include "VulkanBase.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "Tools.h"

struct VertexData {
	float   x, y, z, w;
	float   r, g, b, a;
};

class VulkanExample : public VulkanBase
{

public:

	VulkanExample() {
		// 添加单独的实例级层

		// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
		// Note that on Android this layer requires at least NDK r20
#if defined(_WIN32)
		// 目前我用的NDK r19
		global::enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

		// 添加单独的实例级拓展

		// 这个暂时搁置
		//global::enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		global::enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
		global::enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		global::enabledInstanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

		global::enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	~VulkanExample() {

		m_Image->CleanUp();
		m_UniformBuffer->CleanUp();
		m_VertexBuffer->CleanUp();
		m_IndexBuffer->CleanUp();

		m_VulkanDescriptorPool->CleanUp();
		m_VulkanDescriptorSetLayout->CleanUp();
		m_VulkanPipeline->CleanUp();
		m_VulkanPipelineLayout->CleanUp();
	}

	virtual void Prepare() override {
		PrepareVertices();
		PrepareTextures();
		PrepareUniformBuffer();
		PrepareDescriptorSet();
		m_VulkanPipelineLayout = new VulkanPipelineLayout(m_VulkanDevice, m_VulkanDescriptorSetLayout->m_DescriptorSetLayout);
		CreatePipeline();

		m_CanRender = true;
	}

private:

	void PrepareVertices() {
		// Vertex buffer
		std::vector<float> vertexBuffer =
		{
			  -0.5f, -0.5f, 0.0f,  1.0f ,
			  1.0f, 0.0f, 0.0f,  0.0f  ,
			  0.0f,  0.0f  ,

			   0.5f, -0.5f, 0.0f,  1.0f ,
			   0.0f, 1.0f, 0.0f,  0.0f  ,
			   1.0f,  0.0f  ,

			   0.5f,  0.5f, 0.0f,  1.0f ,
			   0.0f, 0.0f, 1.0f,  0.0f,
			   1.0f,  1.0f  ,
		};
		uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(vertexBuffer[0]);

		m_VertexBuffer = new VulkanBuffer(m_VulkanDevice, vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		UploadBuffer(m_VertexBuffer, vertexBuffer.data(), vertexBufferSize);

		// Index buffer
		std::vector<uint32_t> indexBuffer = { 0, 1, 2 };
		uint32_t indexBufferSize = static_cast<uint32_t>(indexBuffer.size()) * sizeof(uint32_t);

		m_IndexBuffer = new VulkanBuffer(m_VulkanDevice, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		UploadBuffer(m_IndexBuffer, indexBuffer.data(), indexBufferSize);
	}

	void PrepareTextures() {
		uint32_t width = 0, height = 0, dataSize = 0;
		std::vector<char> imageData = GetImageData(GetAssetPath() + "textures/texture.png", 4, &width, &height, nullptr, &dataSize);
		if (imageData.size() == 0) {
			assert(false);
		}

		m_Image = new VulkanImage(m_VulkanDevice, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

		UploadImage(m_Image, imageData.data(), dataSize);
	}

	void PrepareUniformBuffer() {
		std::vector<float> uniformBuffer = {
		2.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f };
		uint32_t uniformBufferSize = static_cast<uint32_t>(uniformBuffer.size()) * sizeof(uniformBuffer[0]);

		m_UniformBuffer = new VulkanBuffer(m_VulkanDevice, uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_UniformBuffer->MapAndCopy(uniformBuffer.data(), uniformBufferSize);
	}

	void PrepareDescriptorSet() {
		m_VulkanDescriptorSetLayout = new VulkanDescriptorSetLayout(m_VulkanDevice);
		m_VulkanDescriptorSetLayout->AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
		m_VulkanDescriptorSetLayout->AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
		m_VulkanDescriptorSetLayout->Create();

		m_VulkanDescriptorPool = new VulkanDescriptorPool(m_VulkanDevice);
		m_VulkanDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		m_VulkanDescriptorPool->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
		m_VulkanDescriptorPool->Create(1);

		m_VulkanDescriptorSet = m_VulkanDescriptorPool->AllocateDescriptorSet(m_VulkanDescriptorSetLayout);

		std::vector<DescriptorSetUpdater*> descriptorSetUpdaters(2);

		descriptorSetUpdaters[0] = new DescriptorSetUpdater(m_VulkanDescriptorSet, 0, 0);
		descriptorSetUpdaters[0]->AddImage(m_Image);

		descriptorSetUpdaters[1] = new DescriptorSetUpdater(m_VulkanDescriptorSet, 1, 0);
		descriptorSetUpdaters[1]->AddBuffer(m_UniformBuffer);

		m_VulkanDevice->UpdateDescriptorSets(descriptorSetUpdaters);
	}

	void CreatePipeline() {
		// std::make_unique 需要C++14的支持，这里使用构造函数更保险
		std::shared_ptr<VulkanShaderModule> vulkanShaderModuleVert = std::shared_ptr<VulkanShaderModule>(new VulkanShaderModule(m_VulkanDevice, GetAssetPath() + "shaders/shader.vert.spv"));
		std::shared_ptr<VulkanShaderModule> vulkanShaderModuleFrag = std::shared_ptr<VulkanShaderModule>(new VulkanShaderModule(m_VulkanDevice, GetAssetPath() + "shaders/shader.frag.spv"));

		PipelineCI pipelineCI;

		pipelineCI.shaderStage.vertShaderModule = vulkanShaderModuleVert;
		pipelineCI.shaderStage.fragShaderModule = vulkanShaderModuleFrag;

		pipelineCI.vertexInputState.vertexLayout.push_back(kVertexFormatFloat32x4);
		pipelineCI.vertexInputState.vertexLayout.push_back(kVertexFormatFloat32x4);
		pipelineCI.vertexInputState.vertexLayout.push_back(kVertexFormatFloat32x2);

		pipelineCI.dynamicState.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		pipelineCI.dynamicState.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

		pipelineCI.Configure(m_VulkanPipelineLayout, m_VulkanRenderPass);

		m_VulkanPipeline = new VulkanPipeline(m_VulkanDevice, pipelineCI);
	}

	void RecordCommandBuffer(VulkanCommandBuffer* vulkanCommandBuffer, VulkanFramebuffer* vulkanFramebuffer) override {
		VkClearValue clearValue = {};
		clearValue.color = { 0.0f, 0.0f, 0.2f, 1.0f };
		clearValue.depthStencil = { 0.0f, 0 };

		VkRect2D area = {};
		area.offset.x = 0;
		area.offset.y = 0;
		area.extent = m_VulkanSwapChain->m_Extent;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_VulkanSwapChain->m_Extent.width);
		viewport.height = static_cast<float>(m_VulkanSwapChain->m_Extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vulkanCommandBuffer->Begin();

		vulkanCommandBuffer->BeginRenderPass(m_VulkanRenderPass, vulkanFramebuffer, area, clearValue);

		vulkanCommandBuffer->SetViewport(viewport);

		vulkanCommandBuffer->SetScissor(area);

		vulkanCommandBuffer->BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline);

		vulkanCommandBuffer->BindVertexBuffer(0, m_VertexBuffer);

		vulkanCommandBuffer->BindIndexBuffer(m_IndexBuffer, VK_INDEX_TYPE_UINT32);

		vulkanCommandBuffer->BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipelineLayout, m_VulkanDescriptorSet);

		vulkanCommandBuffer->DrawIndexed(3, 1, 0, 0, 1);

		vulkanCommandBuffer->EndRenderPass();

		vulkanCommandBuffer->End();
	}

private:

	VulkanPipeline* m_VulkanPipeline;
	VulkanPipelineLayout* m_VulkanPipelineLayout;

	VulkanBuffer* m_VertexBuffer;
	VulkanBuffer* m_IndexBuffer;
	VulkanImage* m_Image;
	VulkanBuffer* m_UniformBuffer;

	VulkanDescriptorPool* m_VulkanDescriptorPool;
	VulkanDescriptorSetLayout* m_VulkanDescriptorSetLayout;
	VulkanDescriptorSet* m_VulkanDescriptorSet;

};

VulkanExample *vulkanExample;

#ifdef _WIN32

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (vulkanExample != nullptr)
	{
		vulkanExample->HandleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	vulkanExample = new VulkanExample();

	vulkanExample->SetupConsole();
	vulkanExample->SetupWindow(hInstance, WndProc);
	vulkanExample->Init();
	vulkanExample->Prepare();
	vulkanExample->Run();

	system("PAUSE");

	delete(vulkanExample);
	return 0;
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

void android_main(android_app* state)
{
	vulkanExample = new VulkanExample();

	androidApp = state;
	androidApp->userData = vulkanExample;
	androidApp->onAppCmd = VulkanExample::HandleAppCommand;
	androidApp->onInputEvent = VulkanExample::HandleAppInput;
	vulkanExample->Run();

	delete(vulkanExample);
}

#endif