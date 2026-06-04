#include "Waterlily/RHIVulkan/VulkanTextureView.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanTexture.hpp"

namespace Wl
{

    void VulkanTextureView::Create(const RHITextureViewDescription& description, VkImageView image_view)
    {
        m_handle = image_view;
        Create(description);
    }

    void VulkanTextureView::Create(const RHITextureViewDescription& description)
    {
        m_description = description;

        VulkanContext& context = VulkanContextGet();
        VulkanTexture* texture = static_cast<VulkanTexture*>(m_description.Texture);

        VkImageViewType image_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
        switch (m_description.Dimension)
        {
            case RHITextureDimension::Dim3D:
                image_type = VK_IMAGE_VIEW_TYPE_3D;
                break;
            case RHITextureDimension::Dim2D:
                image_type = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case RHITextureDimension::Dim1D:
                image_type = VK_IMAGE_VIEW_TYPE_1D;
                break;
            default:
                image_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
                break;
        }

        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = texture->GetHandle();
        image_view_create_info.viewType = image_type;
        image_view_create_info.format = VulkanFormatGet(m_description.Format);
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VulkanImageAspectGet(m_description.Format);
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = m_description.MipLevels;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        WL_VULKAN_CHECK(
                VulkanAPI::vkCreateImageView(context.Device, &image_view_create_info, context.Allocator, &m_handle));
    }

    void VulkanTextureView::Destroy()
    {
        VulkanContext& context = VulkanContextGet();
        VulkanAPI::vkDestroyImageView(context.Device, m_handle, context.Allocator);
    }

}// namespace Wl