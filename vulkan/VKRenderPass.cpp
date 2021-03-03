#include "VKRenderPass.h"
#include "VulkanTools.h"
#include "VKSwapChain.h"
#include "GarbageCollector.h"
#include "ImageManager.h"
#include "ProfilerManager.h"

VKRenderPass::VKRenderPass(VkDevice vkDevice, RenderPassDesc& renderPassDesc, VkImageView swapChainImageView, ImageManager* imageManager, GarbageCollector* gc) :
	m_Device(vkDevice),
	m_GarbageCollector(gc)
{
	m_RenderPassDesc = renderPassDesc;

	// 创建RenderPass

	// Attachments
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	ConfigAttachmentDescriptions(attachmentDescriptions, renderPassDesc);

	// Subpasses
	std::vector<AttachmentReference> attachmentReferences;
	std::vector<VkSubpassDescription> subpassDescriptions;
	ConfigSubpassDescription(attachmentReferences, subpassDescriptions, renderPassDesc);

	// Subpass dependencies
	std::vector<VkSubpassDependency> dependencies;
	ConfigSubpassDependencies(dependencies, renderPassDesc);

	// RenderPass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.flags = 0;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.pAttachments = attachmentDescriptions.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
	renderPassInfo.pSubpasses = subpassDescriptions.data();
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();
	
	VK_CHECK_RESULT(vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass));

	// 创建image
	ConfigImage(renderPassDesc, swapChainImageView, imageManager);

	// 创建FrameBuffer
	std::vector<AttachmentDesc>& attachmentDescs = renderPassDesc.attachmentDescs;
	std::vector<VkImageView> views(attachmentDescs.size());
	for (size_t i = 0; i < attachmentDescs.size(); i++)
	{
		if (m_ImageViews[i])
		{
			views[i] = m_ImageViews[i]->view;
		}
		else
		{
			views[i] = m_SwapChainImageView;
		}
	}

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = nullptr;
	frameBufferCreateInfo.flags = 0;
	frameBufferCreateInfo.renderPass = m_RenderPass;
	frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(views.size());
	frameBufferCreateInfo.pAttachments = views.data();
	frameBufferCreateInfo.width = windowWidth;
	frameBufferCreateInfo.height = windowHeight;
	frameBufferCreateInfo.layers = 1;

	VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &frameBufferCreateInfo, nullptr, &m_Framebuffer));
}

VKRenderPass::~VKRenderPass()
{
	if (m_Device != VK_NULL_HANDLE && m_RenderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
		m_RenderPass = VK_NULL_HANDLE;
	}

	if (m_Device != VK_NULL_HANDLE && m_Framebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(m_Device, m_Framebuffer, nullptr);
		m_Framebuffer = VK_NULL_HANDLE;
	}

	for (size_t i = 0; i < m_Images.size(); i++)
	{
		RELEASE(m_Images[i]);
		RELEASE(m_ImageViews[i]);
	}
}

VkRenderPass VKRenderPass::GetRenderPass()
{
	return m_RenderPass;
}

VkFramebuffer VKRenderPass::GetFramebuffer()
{
	return m_Framebuffer;
}

uint32_t VKRenderPass::GetSubPassIndex()
{
	return m_SubPassIndex;
}

void VKRenderPass::NextSubpass()
{
	m_SubPassIndex++;
}

VKImageView * VKRenderPass::GetInputAttachmentImageView(uint32_t index)
{
	std::vector<SubPassDesc>& subPassDescs = m_RenderPassDesc.subPassDescs;
	auto& inputs = subPassDescs[m_SubPassIndex].inputs;
	return m_ImageViews[inputs[index]];
}

void VKRenderPass::ConfigAttachmentDescriptions(std::vector<VkAttachmentDescription>& attachmentDescriptions, RenderPassDesc& renderPassDesc)
{
	std::vector<AttachmentDesc>& attachmentDescs = renderPassDesc.attachmentDescs;

	attachmentDescriptions.resize(attachmentDescs.size());
	for (size_t atta = 0; atta < attachmentDescs.size(); atta++)
	{
		AttachmentDesc& desc = attachmentDescs[atta];

		attachmentDescriptions[atta].format = desc.format;
		attachmentDescriptions[atta].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions[atta].loadOp = desc.loadOp;
		attachmentDescriptions[atta].storeOp = desc.storeOp;
		attachmentDescriptions[atta].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescriptions[atta].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions[atta].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (renderPassDesc.present == atta)
		{
			attachmentDescriptions[atta].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		else if (renderPassDesc.depthStencil == atta)
		{
			attachmentDescriptions[atta].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		else
		{
			attachmentDescriptions[atta].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}
}

void VKRenderPass::ConfigSubpassDescription(std::vector<AttachmentReference>& attachmentReferences, std::vector<VkSubpassDescription>& subpassDescriptions, RenderPassDesc & renderPassDesc)
{
	std::vector<SubPassDesc>& subPassDescs = renderPassDesc.subPassDescs;

	attachmentReferences.resize(subPassDescs.size());
	subpassDescriptions.resize(subPassDescs.size());
	for (size_t pass = 0; pass < subPassDescs.size(); pass++)
	{
		SubPassDesc& desc = subPassDescs[pass];
		AttachmentReference& reference = attachmentReferences[pass];

		reference.inputs.resize(desc.inputs.size());
		for (size_t atta = 0; atta < desc.inputs.size(); atta++)
		{
			reference.inputs[atta].attachment = desc.inputs[atta];
			reference.inputs[atta].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		reference.colors.resize(desc.colors.size());
		for (size_t atta = 0; atta < desc.colors.size(); atta++)
		{
			reference.colors[atta].attachment = desc.colors[atta];
			reference.colors[atta].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		if (desc.useDepthStencil)
		{
			reference.depthStencil.attachment = renderPassDesc.depthStencil;
			reference.depthStencil.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		subpassDescriptions[pass].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescriptions[pass].colorAttachmentCount = static_cast<uint32_t>(reference.colors.size());
		subpassDescriptions[pass].pColorAttachments = reference.colors.data();
		subpassDescriptions[pass].pDepthStencilAttachment = (desc.useDepthStencil ? &reference.depthStencil : nullptr);
		subpassDescriptions[pass].inputAttachmentCount = static_cast<uint32_t>(reference.inputs.size());
		subpassDescriptions[pass].pInputAttachments = reference.inputs.data();
		subpassDescriptions[pass].preserveAttachmentCount = 0;
		subpassDescriptions[pass].pPreserveAttachments = nullptr;
		subpassDescriptions[pass].pResolveAttachments = nullptr;
	}
}

void VKRenderPass::ConfigSubpassDependencies(std::vector<VkSubpassDependency>& dependencies, RenderPassDesc & renderPassDesc)
{
	dependencies.resize(renderPassDesc.subPassDescs.size() + 1);

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // todo
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[dependencies.size() - 1].srcSubpass = 0;
	dependencies[dependencies.size() - 1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[dependencies.size() - 1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[dependencies.size() - 1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[dependencies.size() - 1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // todo
	dependencies[dependencies.size() - 1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[dependencies.size() - 1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	for (size_t i = 1; i <= dependencies.size() - 2; i++)
	{
		dependencies[i].srcSubpass = static_cast<uint32_t>(i - 1);
		dependencies[i].dstSubpass = static_cast<uint32_t>(i);
		dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
	}
}

void VKRenderPass::ConfigImage(RenderPassDesc& renderPassDesc, VkImageView swapChainImageView, ImageManager* imageManager)
{
	std::vector<AttachmentDesc>& attachmentDescs = renderPassDesc.attachmentDescs;
	std::vector<SubPassDesc>& subPassDescs = renderPassDesc.subPassDescs;

	m_Images.resize(attachmentDescs.size());
	m_ImageViews.resize(attachmentDescs.size());

	for (size_t atta = 0; atta < attachmentDescs.size(); atta++)
	{
		AttachmentDesc& desc = attachmentDescs[atta];

		if (atta == renderPassDesc.present)
		{
			m_Images[atta] = nullptr;
			m_ImageViews[atta] = nullptr;

			m_SwapChainImageView = swapChainImageView;
		}
		else
		{
			VkImageUsageFlags usage = 0;
			VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			for (size_t pass = 0; pass < subPassDescs.size(); pass++)
			{
				SubPassDesc& desc = subPassDescs[pass];

				for (size_t index = 0; index < desc.inputs.size(); index++)
				{
					if (desc.inputs[index] == atta)
					{
						usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
						break;
					}
				}

				for (size_t index = 0; index < desc.colors.size(); index++)
				{
					if (desc.colors[index] == atta)
					{
						usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
						break;
					}
				}
			}

			if (renderPassDesc.depthStencil == atta)
			{
				usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			}

			m_Images[atta] = imageManager->CreateImage(VK_IMAGE_TYPE_2D, desc.format, windowWidth, windowHeight, 1, 1, 1, usage);
			m_ImageViews[atta] = imageManager->CreateView(m_Images[atta]->image, VK_IMAGE_VIEW_TYPE_2D, desc.format, aspectMask, 1, 1, 1);
		}
	}
}

//VKRenderPass * RenderPassCache::GetRenderPass(RenderPassDesc & renderPassDesc)
//{
//	if (m_RenderPassCache.find(renderPassDesc) == m_RenderPassCache.end())
//	{
//		Pipeline* pipeline = CreatePipelineInternal(m_PipelineKey);
//		m_PSOCache[m_PipelineKey] = pipeline;
//		return pipeline->pipeline;
//	}
//	else
//	{
//		return m_RenderPassCache[renderPassDesc];
//	}
//}
