#include "Imgui.h"
#include "GfxDevice.h"
#include "Settings.h"
#include "Shader.h"
#include "Tools.h"
#include "InputManager.h"
#include "ProfilerManager.h"
#include "Image.h"
#include "Buffer.h"
#include <algorithm>

Imgui::Imgui()
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

#if VK_USE_PLATFORM_ANDROID_KHR
	ImGuiStyle* style = &ImGui::GetStyle();
	style->ScaleAllSizes(3.0f);
#endif

	auto& device = GetGfxDevice();

	// font texture, use the default

	int width, height;
	unsigned char* pixels = NULL;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	uint64_t dataSize = width * height * 4 * sizeof(char);

	m_FontImage = device.CreateImage(VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1, 1);
	device.UpdateImage(m_FontImage, pixels, dataSize, { { { 0 } } });

	// Vertex buffer

	uint64_t vertexBufferSize = m_MaxVertexCount * sizeof(ImDrawVert);
	m_VertexBuffer = device.CreateBuffer(kBufferUsageVertex, kMemoryPropertyHostVisible, vertexBufferSize); // 因为需要每帧更新，这里使用HostVisible，由于一帧内需要多次更新，不使用HostCoherent

	// Index buffer

	VkDeviceSize indexBufferSize = m_MaxIndexCount * sizeof(ImDrawIdx);
	m_IndexBuffer = device.CreateBuffer(kBufferUsageIndex, kMemoryPropertyHostVisible, indexBufferSize); // 因为需要每帧更新，这里使用HostVisible，由于一帧内需要多次更新，不使用HostCoherent

	// Shader

	m_Shader = new Shader("ColorShader");
	m_Shader->LoadSPV(AssetPath + "shaders/imgui/imgui.vert.spv", AssetPath + "shaders/imgui/imgui.frag.spv");

	GpuParameters parameters;
	{
		GpuParameters::TextureParameter texture("Base", 0, VK_SHADER_STAGE_FRAGMENT_BIT);
		parameters.textureParameters.push_back(texture);
	}
	m_Shader->CreateGpuProgram(parameters);

	RenderState renderState;

	renderState.rasterizationState.cullMode = VK_CULL_MODE_NONE;
	renderState.rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	renderState.blendStates[0].blendEnable = VK_TRUE;
	renderState.blendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	renderState.blendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	renderState.blendStates[0].colorBlendOp = VK_BLEND_OP_ADD;
	renderState.blendStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	renderState.blendStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	renderState.blendStates[0].alphaBlendOp = VK_BLEND_OP_ADD;

	renderState.depthStencilState.depthTestEnable = VK_FALSE;
	renderState.depthStencilState.depthWriteEnable = VK_FALSE;

	m_Shader->SetRenderState(renderState);

	// VertexDescription

	m_VertexDes.formats = { VK_FORMAT_R32G32_SFLOAT ,VK_FORMAT_R32G32_SFLOAT ,VK_FORMAT_R8G8B8A8_UNORM };
	m_VertexDes.offsets = { 0, VkFormatToSize(VK_FORMAT_R32G32_SFLOAT),VkFormatToSize(VK_FORMAT_R32G32_SFLOAT) + VkFormatToSize(VK_FORMAT_R32G32_SFLOAT) };
	m_VertexDes.stride = VkFormatToSize(VK_FORMAT_R32G32_SFLOAT) + VkFormatToSize(VK_FORMAT_R32G32_SFLOAT) + VkFormatToSize(VK_FORMAT_R8G8B8A8_UNORM);
}

Imgui::~Imgui()
{
	ImGui::DestroyContext();
	GetGfxDevice().ReleaseImage(m_FontImage);
	RELEASE(m_FontImage);
	RELEASE(m_VertexBuffer);
	RELEASE(m_IndexBuffer);
	RELEASE(m_Shader);
}

void Imgui::Prepare(float deltaTime)
{
	PROFILER(Imgui_Prepare);

	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
	io.DeltaTime = deltaTime;

#if defined(_WIN32)
	io.MousePos = ImVec2(inputManager.pos.x, inputManager.pos.y);
	io.MouseDown[0] = inputManager.key_MouseLeft;
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	io.MousePos = ImVec2(inputManager.pos0.x, inputManager.pos0.y);
	io.MouseDown[0] = inputManager.count != 0;
#endif

	ImGui::NewFrame();
}

void Imgui::Tick()
{
	PROFILER(Imgui_Tick);

	auto& device = GetGfxDevice();

	ImDrawData* imDrawData = ImGui::GetDrawData();

	if (!imDrawData) return;

	if ((imDrawData->TotalVtxCount == 0) || (imDrawData->TotalIdxCount == 0)) {
		return;
	}

	uint32_t vertexOffset = 0;
	uint32_t indexOffset = 0;

	for (int n = 0; n < imDrawData->CmdListsCount; n++) {
		const ImDrawList* cmd_list = imDrawData->CmdLists[n];

		device.UpdateBuffer(m_VertexBuffer, cmd_list->VtxBuffer.Data, vertexOffset, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		device.UpdateBuffer(m_IndexBuffer, cmd_list->IdxBuffer.Data, indexOffset, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

		vertexOffset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
		indexOffset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
	}

	device.FlushBuffer(m_VertexBuffer);
	device.FlushBuffer(m_IndexBuffer);
}

void Imgui::Draw()
{
	PROFILER(Imgui_Draw);

	auto& device = GetGfxDevice();

	ImDrawData* imDrawData = ImGui::GetDrawData();

	if ((!imDrawData) || (imDrawData->CmdListsCount == 0))
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	GpuProgram* gpuProgram = m_Shader->GetGpuProgram();

	device.SetPass(gpuProgram, m_Shader->GetRenderState(), nullptr);

	GetGfxDevice().BindImage(gpuProgram, 0, m_FontImage);

	device.BindMeshBuffer(m_VertexBuffer, m_IndexBuffer, &m_VertexDes, VK_INDEX_TYPE_UINT16);

	// PushConstant
	float scale[2];
	scale[0] = 2.0f / imDrawData->DisplaySize.x;
	scale[1] = 2.0f / imDrawData->DisplaySize.y;
	float translate[2];
	translate[0] = -1.0f - imDrawData->DisplayPos.x * scale[0];
	translate[1] = -1.0f - imDrawData->DisplayPos.y * scale[1];
	device.PushConstants(gpuProgram, scale, sizeof(float) * 0, sizeof(float) * 2);
	device.PushConstants(gpuProgram, translate, sizeof(float) * 2, sizeof(float) * 2);

	int vertexOffset = 0;
	int indexOffset = 0;

	for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
	{
		const ImDrawList* cmd_list = imDrawData->CmdLists[i];
		for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];

			Rect2D area(std::max((int32_t)(pcmd->ClipRect.x), 0), 
				std::max((int32_t)(pcmd->ClipRect.y), 0), 
				(uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x), 
				(uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y));

			device.SetScissor(area);

			device.DrawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + indexOffset, pcmd->VtxOffset + vertexOffset, 0);
		}
		vertexOffset += cmd_list->VtxBuffer.Size;
		indexOffset += cmd_list->IdxBuffer.Size;
	}
}