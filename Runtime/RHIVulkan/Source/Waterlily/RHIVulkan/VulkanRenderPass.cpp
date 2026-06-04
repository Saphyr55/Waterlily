#include "Waterlily/RHIVulkan/VulkanRenderPass.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    void VulkanRenderPass::Create()
    {
        Array<VkAttachmentDescription> attachments(m_description.ColorAttachmentDecriptions.size() + 1);
        for (const RHIColorAttachmentDescription& attachment_description: m_description.ColorAttachmentDecriptions)
        {
            VkAttachmentDescription colorAttachmentDescription = {};
            colorAttachmentDescription.format = VulkanFormatGet(attachment_description.Format);
            colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentDescription.loadOp = VulkanLoadOpGet(attachment_description.LoadOp);
            colorAttachmentDescription.storeOp = VulkanStoreOpGet(attachment_description.StoreOp);
            colorAttachmentDescription.stencilLoadOp = VulkanLoadOpGet(attachment_description.StencilLoadOp);
            colorAttachmentDescription.stencilStoreOp = VulkanStoreOpGet(attachment_description.StencilStoreOp);
            colorAttachmentDescription.initialLayout = VulkanTextureLayoutGet(attachment_description.InitialLayout);
            colorAttachmentDescription.finalLayout = VulkanTextureLayoutGet(attachment_description.FinalLayout);
            attachments.Append(colorAttachmentDescription);
        }

        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentReference;

        if (m_description.DepthAttachmentDescription.HasValue())
        {
            RHIDepthAttachmentDescription& depthAttachmentDesc = *m_description.DepthAttachmentDescription;

            VkImageLayout layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            if (RHIFormatIsDepthStencil(depthAttachmentDesc.Format))
            {
                layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            else if (RHIFormatIsDepth(depthAttachmentDesc.Format))
            {
                layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            }
            else
            {
                WL_CRASH("Not supported format for depth or depth-stencil.");
            }

            VkAttachmentDescription depthAttachmentDescription = {};
            depthAttachmentDescription.format = VulkanFormatGet(depthAttachmentDesc.Format);
            depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachmentDescription.loadOp = VulkanLoadOpGet(depthAttachmentDesc.LoadOp);
            depthAttachmentDescription.storeOp = VulkanStoreOpGet(depthAttachmentDesc.StoreOp);
            depthAttachmentDescription.stencilLoadOp = VulkanLoadOpGet(depthAttachmentDesc.StencilLoadOp);
            depthAttachmentDescription.stencilStoreOp = VulkanStoreOpGet(depthAttachmentDesc.StencilStoreOp);
            depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachmentDescription.finalLayout = layout;

            VkAttachmentReference depthAttachmentReference = {};
            depthAttachmentReference.attachment = 1;
            depthAttachmentReference.layout = layout;

            subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
            attachments.Append(depthAttachmentDescription);
        }

        VkSubpassDependency subpassDependency = {};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency depthSubpassDependency = {};
        depthSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depthSubpassDependency.dstSubpass = 0;
        depthSubpassDependency.srcStageMask =
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthSubpassDependency.srcAccessMask = 0;
        depthSubpassDependency.dstStageMask =
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthSubpassDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        Array<VkSubpassDependency> dependencies(attachments.size());
        for (size_t i = 0; i < attachments.size() - 1; i++)
        {
            dependencies.Append(subpassDependency);
        }

        if (m_description.DepthAttachmentDescription.HasValue())
        {
            dependencies.Append(depthSubpassDependency);
        }

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = attachments.size();
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.dependencyCount = dependencies.size();
        renderPassCreateInfo.pDependencies = dependencies.data();

        WL_VULKAN_CHECK(VulkanAPI::vkCreateRenderPass(m_context.Device,
                                                      &renderPassCreateInfo,
                                                      m_context.Allocator,
                                                      &m_renderPass));
    }

    void VulkanRenderPass::Destroy()
    {
        VulkanAPI::vkDestroyRenderPass(m_context.Device, m_renderPass, m_context.Allocator);
    }

}// namespace Wl