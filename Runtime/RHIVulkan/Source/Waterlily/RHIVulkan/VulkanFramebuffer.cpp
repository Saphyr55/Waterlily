#include "Waterlily/RHIVulkan/VulkanFramebuffer.hpp"

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Cast.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderPass.hpp"
#include "Waterlily/RHIVulkan/VulkanTexture.hpp"
#include "Waterlily/RHIVulkan/VulkanTextureView.hpp"

namespace Wl
{

    VulkanFramebuffer::VulkanFramebuffer(VulkanContext& context, const RHIFramebufferDescription& description)
        : m_description(description)
        , m_context(context)
        , m_handle(VK_NULL_HANDLE)
    {
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        if (m_handle)
        {
            Destroy();
        }
    }

    VkFramebuffer& VulkanFramebuffer::GetHandle()
    {
        return m_handle;
    }

    const VkFramebuffer& VulkanFramebuffer::GetHandle() const
    {
        return m_handle;
    }

    void VulkanFramebuffer::Create()
    {
        VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(m_description.RenderPass);

        Array<VkImageView> attachments(m_description.Attachments.size());
        for (RHITextureView* textureView: m_description.Attachments)
        {
            VulkanTextureView* vulkanTextureView = static_cast<VulkanTextureView*>(textureView);
            WL_CHECK(vulkanTextureView);
            attachments.Append(vulkanTextureView->GetHandle());
        }

        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = vulkanRenderPass->GetHandle();
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = m_description.Width;
        createInfo.height = m_description.Height;
        createInfo.layers = m_description.Layers;

        WL_VULKAN_CHECK(
                VulkanAPI::vkCreateFramebuffer(m_context.Device, &createInfo, m_context.Allocator, &m_handle));
    }

    void VulkanFramebuffer::Destroy()
    {
        VulkanAPI::vkDestroyFramebuffer(m_context.Device, m_handle, m_context.Allocator);
        m_handle = VK_NULL_HANDLE;
    }

}// namespace Wl