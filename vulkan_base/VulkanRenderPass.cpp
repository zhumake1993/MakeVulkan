#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
#include "Tools.h"

VulkanRenderPass::VulkanRenderPass(VulkanDevice * vulkanDevice, VkFormat format):
	m_VulkanDevice(vulkanDevice)
{
	// This example will use a single render pass with one subpass

	// Descriptors for the attachments used by this renderpass
	std::vector<VkAttachmentDescription> attachmentDescriptions(1);

	// Color attachment
	attachmentDescriptions[0].flags = 0;
	attachmentDescriptions[0].format = format;                                  // Use the color format selected by the swapchain
	attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;                                 // We don't use multi sampling in this example
	attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear this attachment at the start of the render pass
	attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                          // Keep its contents after the render pass is finished (for displaying it)
	attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // We don't use stencil, so don't care for load
	attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // Same for store
	attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
	attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;                   // Layout to which the attachment is transitioned when the render pass is finished
																					// As we want to present the color buffer to the swapchain, we transition to PRESENT_KHR

	// Setup attachment references
	VkAttachmentReference colorReference = {};
	colorReference.attachment = 0;                                    // Attachment 0 is color
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Attachment layout used as color during the subpass

	// Setup a single subpass reference
	VkSubpassDescription subpassDescription = {};
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;                            // Input attachments can be used to sample from contents of a previous subpass
	subpassDescription.pInputAttachments = nullptr;                         // (Input attachments not used by this example)
	subpassDescription.colorAttachmentCount = 1;                            // Subpass uses one color attachment
	subpassDescription.pColorAttachments = &colorReference;                 // Reference to the color attachment in slot 0
	subpassDescription.pResolveAttachments = nullptr;                       // Resolve attachments are resolved at the end of a sub pass and can be used for e.g. multi sampling
	subpassDescription.pDepthStencilAttachment = nullptr;           // Reference to the depth attachment in slot 1
	subpassDescription.preserveAttachmentCount = 0;                         // Preserved attachments can be used to loop (and preserve) attachments through subpasses
	subpassDescription.pPreserveAttachments = nullptr;                      // (Preserve attachments not used by this example)

	// Setup subpass dependencies
	// These will add the implicit ttachment layout transitionss specified by the attachment descriptions
	// The actual usage layout is preserved through the layout specified in the attachment reference
	// Each subpass dependency will introduce a memory and execution dependency between the source and dest subpass described by
	// srcStageMask, dstStageMask, srcAccessMask, dstAccessMask (and dependencyFlags is set)
	// Note: VK_SUBPASS_EXTERNAL is a special constant that refers to all commands executed outside of the actual renderpass)
	std::vector<VkSubpassDependency> dependencies(2);

	// First dependency at the start of the renderpass
	// Does the transition from final to initial layout
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;                             // Producer of the dependency
	dependencies[0].dstSubpass = 0;                                               // Consumer is our single subpass that will wait for the execution depdendency
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Match our pWaitDstStageMask when we vkQueueSubmit
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // is a loadOp stage for color attachments
	dependencies[0].srcAccessMask = 0;                                            // semaphore wait already does memory dependency for us
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;         // is a loadOp CLEAR access mask for color attachments
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Second dependency at the end the renderpass
	// Does the transition from the initial to the final layout
	// Technically this is the same as the implicit subpass dependency, but we are gonna state it explicitly here
	dependencies[1].srcSubpass = 0;                                               // Producer of the dependency is our single subpass
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;                             // Consumer are all commands outside of the renderpass
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // is a storeOp stage for color attachments
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;          // Do not block any subsequent work
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;         // is a storeOp `STORE` access mask for color attachments
	dependencies[1].dstAccessMask = 0;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create the actual renderpass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.flags = 0;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());  // Number of attachments used by this render pass
	renderPassInfo.pAttachments = attachmentDescriptions.data();                            // Descriptions of the attachments used by the render pass
	renderPassInfo.subpassCount = 1;                                             // We only use one subpass in this example
	renderPassInfo.pSubpasses = &subpassDescription;                             // Description of that subpass
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size()); // Number of subpass dependencies
	renderPassInfo.pDependencies = dependencies.data();                          // Subpass dependencies used by the render pass

	VK_CHECK_RESULT(vkCreateRenderPass(m_VulkanDevice->m_LogicalDevice, &renderPassInfo, nullptr, &m_RenderPass));
}

VulkanRenderPass::~VulkanRenderPass()
{
}

void VulkanRenderPass::CleanUp()
{
	if (m_VulkanDevice && m_VulkanDevice->m_LogicalDevice != VK_NULL_HANDLE && m_RenderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(m_VulkanDevice->m_LogicalDevice, m_RenderPass, nullptr);
		m_RenderPass = VK_NULL_HANDLE;
	}
}
