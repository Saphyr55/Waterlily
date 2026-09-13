#pragma once

#include "Waterlily/RHI/TextureView.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanTextureView : public RHITextureView
    {
    public:
        VkImageView& GetHandle()
        {
            return m_handle;
        }

        void Create(const RHITextureViewDescription& description, VkImageView imageView);
        void Create(const RHITextureViewDescription& description);
        void Create()
        {
            Create(m_description, m_handle);
        }

        void Destroy();

    public:
        VulkanTextureView() = default;
        VulkanTextureView(const RHITextureViewDescription& description, VkImageView imageView = VK_NULL_HANDLE)
            : m_handle(imageView)
        {
            m_description = description;
        }

        ~VulkanTextureView() = default;

    private:
        VkImageView m_handle = VK_NULL_HANDLE;
    };

}// namespace Wl
