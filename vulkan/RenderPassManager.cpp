#include "RenderPassManager.h"
#include "VulkanTools.h"
#include "ImageManager.h"

AttachmentVulkan::AttachmentVulkan(int typeMask, VkFormat format, uint32_t width, uint32_t height, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp)
	: Attachment(typeMask, format, width, height, loadOp, storeOp)
{
}

AttachmentVulkan::~AttachmentVulkan()
{
}

RenderPassManager::RenderPassManager(VkDevice vkDevice)
	: m_Device(vkDevice)
{
}

RenderPassManager::~RenderPassManager()
{
}

VKRenderPass * RenderPassManager::GetRenderPass(RenderPassKey & key)
{
	VKRenderPass* renderPass = m_RenderPassPool.Get(key);
	if (renderPass)
	{
		return renderPass;
	}
	else
	{
		return CreateRenderPass(key);
	}
}

void RenderPassManager::ReleaseRenderPass(RenderPassKey& key, VKRenderPass * renderPass)
{
	m_RenderPassPool.Add(key, renderPass);
}

struct AttachmentReference
{
	std::vector<VkAttachmentReference> inputs;
	std::vector<VkAttachmentReference> colors;
	VkAttachmentReference depthStencil;
};

VKRenderPass * RenderPassManager::CreateRenderPass(RenderPassKey & key)
{
	VKRenderPass* renderPass = new VKRenderPass(m_Device);

	// Attachments
	std::vector<VkAttachmentDescription> attachmentDescriptions(key.attachments.size());
	for (size_t i = 0; i < attachmentDescriptions.size(); i++)
	{
		attachmentDescriptions[i].flags = 0;
		attachmentDescriptions[i].format = key.attachments[i].format;
		attachmentDescriptions[i].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions[i].loadOp = key.attachments[i].loadOp;
		attachmentDescriptions[i].storeOp = key.attachments[i].storeOp;
		attachmentDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (key.attachments[i].typeMask & kAttachmentColor)
		{
			attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		else if (key.attachments[i].typeMask & kAttachmentDepth)
		{
			attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		else if (key.attachments[i].typeMask & kAttachmentSwapChain)
		{
			attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		else
		{
			LOGE("wrong attachment typeMask");
		}
	}

	// Subpasses
	std::vector<AttachmentReference> attachmentReferences(key.subpasses.size());
	std::vector<VkSubpassDescription> subpassDescriptions(key.subpasses.size());
	for (size_t pass = 0; pass < subpassDescriptions.size(); pass++)
	{
		RenderPassKey::SubpassKey& subpass = key.subpasses[pass];
		AttachmentReference& reference = attachmentReferences[pass];

		reference.inputs.resize(subpass.inputs.size());
		for (size_t i = 0; i < subpass.inputs.size(); i++)
		{
			reference.inputs[i].attachment = subpass.inputs[i];
			reference.inputs[i].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		reference.colors.resize(subpass.colors.size());
		for (size_t i = 0; i < subpass.colors.size(); i++)
		{
			reference.colors[i].attachment = subpass.colors[i];
			reference.colors[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		if (subpass.depth >= 0)
		{
			reference.depthStencil.attachment = subpass.depth;
			reference.depthStencil.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		subpassDescriptions[pass].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescriptions[pass].colorAttachmentCount = static_cast<uint32_t>(reference.colors.size());
		subpassDescriptions[pass].pColorAttachments = reference.colors.data();
		subpassDescriptions[pass].pDepthStencilAttachment = (subpass.depth >= 0 ? &reference.depthStencil : nullptr);
		subpassDescriptions[pass].inputAttachmentCount = static_cast<uint32_t>(reference.inputs.size());
		subpassDescriptions[pass].pInputAttachments = reference.inputs.data();
		subpassDescriptions[pass].preserveAttachmentCount = 0;
		subpassDescriptions[pass].pPreserveAttachments = nullptr;
		subpassDescriptions[pass].pResolveAttachments = nullptr;
	}

	// Subpass dependencies
	std::vector<VkSubpassDependency> dependencies(key.subpasses.size() + 1);

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

	// RenderPass
	VkRenderPassCreateInfo renderPassCI = {};
	renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCI.pNext = nullptr;
	renderPassCI.flags = 0;
	renderPassCI.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassCI.pAttachments = attachmentDescriptions.data();
	renderPassCI.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
	renderPassCI.pSubpasses = subpassDescriptions.data();
	renderPassCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassCI.pDependencies = dependencies.data();

	VK_CHECK_RESULT(vkCreateRenderPass(m_Device, &renderPassCI, nullptr, &renderPass->renderPass));

	return renderPass;
}

VkImageView RenderPassVulkan::GetInputAttachmentImageView(uint32_t inputIndex)
{
	int attachmentIndex = m_Subpasses[m_SubpassIndex].inputs[inputIndex];
	AttachmentVulkan* attachmentVK = static_cast<AttachmentVulkan*>(m_Attachments[attachmentIndex]);
	return attachmentVK->m_Image->view;
}
